#include "demo_objects.hpp"

#include "core_automation/interfaces/automation_interface.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_data_model/reflection/reflected_tree_model.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/metadata/meta_impl.hpp"
#include "core_reflection/metadata/meta_utilities.hpp"
#include "core_reflection/interfaces/i_reflection_controller.hpp"
#include "core_string_utils/string_utils.hpp"
#include "core_string_utils/file_path.hpp"
#include "testing/reflection_objects_test/test_objects.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_serialization/i_file_system.hpp"
#include "core_command_system/i_command_manager.hpp"

#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include <stdio.h>
#include <codecvt>


namespace wgt
{
namespace
{
	enum class ModelPropertyValueType : uint8_t
	{
		String = 0,
		Boolean,
		Integer,
		Double,
		EnumList,
		Vector2,
		Vector3,
		Vector4,
	};

    template< typename T >
    T getObjectProperty( int index, const GenericListT<GenericObjectPtr>& objects, const char* name, T&& defaultValue )
    {
        assert( index >= 0 && static_cast<size_t>( index ) < objects.size() );
        auto genericObject = objects[index];
    
        const auto accessor = genericObject->findProperty( name );
        if ( accessor.isValid() )
        {
            T value;
            bool isOk = genericObject->get( name, value );
            assert( isOk );
            return value;
        }

        return std::forward<T>( defaultValue );
    }
}

class DemoObjectsEnvCom : public IEnvComponent
{
	DEFINE_EC_GUID
public:
	DemoObjectsEnvCom() : index_( -1 ) {}
	virtual ~DemoObjectsEnvCom() {}

	GenericListT<GenericObjectPtr> objList_;
	int index_;
};

DECLARE_EC_GUID( DemoObjectsEnvCom, 0x157cbf2eu, 0x940a4c9au, 0x97c49525u, 0x180b1f23u );

DemoObjects::DemoObjects()
	: objects_(nullptr)
	, pEnvChangeHelper_( new ValueChangeNotifier< IListModel* >( nullptr ) )
{
}

DemoObjects::~DemoObjects()
{
}

bool DemoObjects::init( IComponentContext & contextManager )
{
	auto definitionManager = contextManager.queryInterface< IDefinitionManager >();
	auto fileSystem = contextManager.queryInterface< IFileSystem >();
	auto controller = contextManager.queryInterface< IReflectionController >();
	auto envManager = contextManager.queryInterface< IEnvManager >();
	auto comMngr = contextManager.queryInterface< ICommandManager >();

	if ((definitionManager == nullptr) || (controller == nullptr) || 
			(envManager == nullptr) || (fileSystem == nullptr) || (comMngr == nullptr))
	{
		return false;
	}
	pDefManager_ = definitionManager;
	controller_ = controller;
	envManager_ = envManager;
	fileSystem_ = fileSystem;

	helper_.init( &comMngr->selectionContext(), nullSelection_ );

	envManager_->registerListener( this );
	return true;
}

bool DemoObjects::fini()
{
	envManager_->deregisterListener( this );
	return true;
}

const IValueChangeNotifier * DemoObjects::currentIndexSource() const
{
	return &helper_;
}

const IValueChangeNotifier * DemoObjects::currentListSource() const
{
	return pEnvChangeHelper_.get();
}

ObjectHandle DemoObjects::createObject( Vector3 pos )
{
	RefObjectId id = RefObjectId::generate();
	GenericObjectPtr genericObject = GenericObject::create( *pDefManager_, id );
	genericObject->set( "name", std::string("object_") + id.toString() );
	genericObject->set( "position", pos );
	genericObject->set("rotation", Vector3(0.0f, 0.0f, 0.0f));
	genericObject->set("scale", Vector3(1.0f, 1.0f, 1.0f));
	genericObject->set("visible", true);
	genericObject->set( "map1", std::string("logo.png"));
	objects_->objList_.push_back( genericObject );
	return genericObject;
}

void DemoObjects::undoCreateObject( const ObjectHandle& params, Variant result )
{
	assert( result.canCast<ObjectHandle>() );
	GenericObjectPtr genericObject = result.cast<ObjectHandle>().getBase<GenericObject>();
	auto it = std::find( objects_->objList_.begin(), objects_->objList_.end(), genericObject);
	assert( it != objects_->objList_.end() );
	objects_->objList_.erase( it );
}

void DemoObjects::redoCreateObject( const ObjectHandle& params, Variant result )
{
	assert( result.canCast<ObjectHandle>() );
	GenericObjectPtr genericObject = result.cast<ObjectHandle>().getBase<GenericObject>();
	objects_->objList_.push_back( genericObject );
}

void DemoObjects::onAddEnv(IEnvState* state)
{
	ENV_STATE_ADD( DemoObjectsEnvCom, ec );
	loadDemoData( state->description(), ec );
}

void DemoObjects::onRemoveEnv(IEnvState* state)
{
	ENV_STATE_REMOVE( DemoObjectsEnvCom, ec );
	if (objects_ == ec)
	{
		objects_ = nullptr;
		helper_.value( nullSelection_ );
	}
}

void DemoObjects::onSelectEnv(IEnvState* state)
{
	ENV_STATE_QUERY( DemoObjectsEnvCom, ec );
	if (objects_ != ec)
	{
		objects_ = ec;
		pEnvChangeHelper_->value( &(objects_->objList_) );
		helper_.value( (objects_->index_ >= 0) ? objects_->objList_[objects_->index_] : nullSelection_ );
	}
}

const ITreeModel* DemoObjects::getTreeModel() const
{
	treeModel_.reset(
	new ReflectedTreeModel(helper_.value(), *pDefManager_, controller_));
	return treeModel_.get();
}

const IListModel * DemoObjects::getListModel() const
{
	return &(objects_->objList_);
}

void DemoObjects::updateRootObject( int index )
{
	if (objects_)
	{
		objects_->index_ = index;
		helper_.value((objects_->index_ >= 0) ? objects_->objList_[index] : nullSelection_);
	}
}

const int DemoObjects::rootObjectIndex() const
{
	if (objects_ == nullptr)
	{
		return 0;
	}
	return objects_->index_;
}

// TODO:remove tiny xml dependency and use our own serialization stuff to handle this
bool DemoObjects::loadDemoData( const char* name, DemoObjectsEnvCom* objects )
{
	std::string file = PROJECT_RESOURCE_FOLDER;
	file += name;
	file += ".xml";

	if (!fileSystem_->exists( file.c_str() ))
	{
		NGT_ERROR_MSG("Could not find file %s", file.c_str());
		return false;
	}

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile( file.c_str() ) != tinyxml2::XML_SUCCESS || doc.ErrorID())
	{
		NGT_ERROR_MSG( "Failed to load %s\n", file.c_str() );
		return false;
	}

	auto root = doc.RootElement();
	auto node = root->FirstChildElement( "object" );
	while (node != nullptr)
	{
		auto genericObject = GenericObject::create( *pDefManager_ );
		objects->objList_.push_back( genericObject );
		populateDemoObject( genericObject, *node );
		node = node->NextSiblingElement( "object" );
	}
	return true;
}

void DemoObjects::populateDemoObject( GenericObjectPtr & genericObject, const tinyxml2::XMLNode& objectNode )
{
	auto propertyNode = objectNode.FirstChildElement( "property" );
	while (propertyNode != nullptr)
	{
		uint8_t valueType = 0;
		const char * propertyName = propertyNode->Attribute( "name" );
		auto attribute = propertyNode->FindAttribute( "type" );
		if (attribute != nullptr)
		{
			valueType = static_cast<uint8_t>( attribute->UnsignedValue() );
		}
		std::string tmp("");
		const char * value = propertyNode->GetText();
		if (value != nullptr)
		{
			tmp = value;
		}
		switch( valueType )
		{
		case (uint8_t)ModelPropertyValueType::Boolean:
			{
				bool realValue = false;
				tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
				bool isOk = tinyxml2::XMLUtil::ToBool( tmp.c_str(), &realValue );
				assert( isOk );
				genericObject->set( propertyName, realValue );
			}
			break;
		case (uint8_t)ModelPropertyValueType::Integer:
			{
				int realValue = -1;
				tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
				bool isOk = tinyxml2::XMLUtil::ToInt( tmp.c_str(), &realValue );
				assert( isOk );
				genericObject->set( propertyName, realValue );
			}
			break;
		case (uint8_t)ModelPropertyValueType::Double:
			{
				double realValue = -1.0;
				tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
				bool isOk = tinyxml2::XMLUtil::ToDouble( tmp.c_str(), &realValue );
				assert( isOk );
				genericObject->set( propertyName, realValue );
			}
			break;
		case (uint8_t)ModelPropertyValueType::EnumList:
			break;
		case (uint8_t)ModelPropertyValueType::Vector2:
			{
				tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
				float d[2] = {0.0f, 0.0f};
				char * pch = nullptr;
				pch = strtok (const_cast<char *>(tmp.c_str()),",");
				int i = 0;
				while (pch != nullptr)
				{
					float v = static_cast<float>( atof( pch ) );
					d[i++] = v;
					pch = strtok (nullptr, ",");
				}
				assert( i == 2 );
				Vector2 realValue( d[0], d[1] );
				genericObject->set( propertyName, realValue );
			}
			break;
		case (uint8_t)ModelPropertyValueType::Vector3:
			{
				tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
				float d[3] = {0.0f, 0.0f, 0.0f};
				char * pch = nullptr;
				pch = strtok (const_cast<char *>(tmp.c_str()),",");
				int i = 0;
				while (pch != nullptr)
				{
					float v = static_cast<float>( atof( pch ) );
					d[i++] = v;
					pch = strtok (nullptr, ",");
				}
				assert( i == 3 );
				Vector3 realValue( d[0], d[1], d[2] );
				genericObject->set( propertyName, realValue );
			}
			break;
		case (uint8_t)ModelPropertyValueType::Vector4:
			{
				tmp.erase(remove_if(tmp.begin(), tmp.end(), isspace), tmp.end());
				float d[4] = {0.0f, 0.0f, 0.0f, 0.0f};
				char * pch = nullptr;
				pch = strtok (const_cast<char *>(tmp.c_str()),",");
				int i = 0;
				while (pch != nullptr)
				{
					float v = static_cast<float>( atof( pch ) );
					d[i++] = v;
					pch = strtok (nullptr, ",");
				}
				assert( i == 4 );
				Vector4 realValue( d[0], d[1], d[2], d[3] );
				genericObject->set( propertyName, realValue );
			}
			break;
		default:
			{
				genericObject->set( propertyName, tmp );
			}
			break;
		}

		propertyNode = propertyNode->NextSiblingElement( "property" );
	}

}


std::string DemoObjects::getObjectTexture( int index )
{
	assert( objects_ );
	if (index > -1 && index < (int)objects_->objList_.size())
	{
		return getObjectProperty<std::string>( index, objects_->objList_, "map1", "" );
	}
	return "";
}

void DemoObjects::setTexture( int index, std::string currFilePath, std::string newFilePath )
{
	if (index > -1 && index < (int)objects_->objList_.size())
	{
		objects_->objList_[index]->set( "map1", newFilePath );
	}
}

void DemoObjects::undoSetTexture( const ObjectHandle& params, Variant result )
{
	ReflectedMethodParameters *args = params.getBase<ReflectedMethodParameters>();
	int index = (*args)[0].cast<int>();
	std::string prevFilePath = (*args)[1].cast<std::string>();

	if (index > -1 && index < (int)objects_->objList_.size())
	{
		objects_->objList_[index]->set( "map1", prevFilePath );
	}
}

void DemoObjects::redoSetTexture( const ObjectHandle& params, Variant result )
{
	ReflectedMethodParameters *args = params.getBase<ReflectedMethodParameters>();

	int index = (*args)[0].cast<int>();
	std::string newFilePath = (*args)[2].cast<std::string>();

	if (index > -1 && index < (int)objects_->objList_.size())
	{
		objects_->objList_[index]->set( "map1", newFilePath );
	}
}


} // end namespace wgt
