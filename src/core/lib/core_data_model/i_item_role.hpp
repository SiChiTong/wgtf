#ifndef I_ITEM_ROLE_HPP
#define I_ITEM_ROLE_HPP

#include "wg_types/hash_utilities.hpp"

#define ITEMROLE( ROLE )														\
namespace ItemRole {															\
	namespace {																	\
		const char * ROLE##Name = #ROLE;										\
		const Id ROLE##Id = ::wgt::ItemRole::compute( #ROLE );				\
	}																			\
}

namespace wgt
{

namespace ItemRole
{
	typedef uint64_t Id;

	inline Id compute( const char * roleName )
	{
		return HashUtilities::compute( roleName );
	}
}

// DEPRECATED
// Add new role types here
#define ITEM_ROLES																\
	/* Tree Adapter */															\
	X(ChildModel, childModel)													\
	X(ColumnModel, columnModel)												\
	X(HasChildren, hasChildren)												\
	X(Expanded, expanded)														\
	X(ParentIndex, parentIndex)												\
	X(Selected, selected)														\
	X(IndexPath, indexPath)													\
	/* Component Extension */													\
	X(Component, component)													\
	X(ValueType, valueType)													\
	X(KeyType, keyType)														\
	X(IsAction, isAction)														\
	X(IsEnum, isEnum)															\
	X(IsThumbnail, isThumbnail)												\
	X(IsSlider, isSlider)														\
	X(IsColor, isColor)														\
	X(IsUrl, isUrl)															\
	/* Value Extension */														\
	X(Value, value)															\
	X(Key, key)																\
	X(MinValue, minValue)														\
	X(MaxValue, maxValue)														\
	X(StepSize, stepSize)														\
	X(Decimals, decimals)														\
	X(EnumModel, enumModel)													\
	X(Definition, definition)													\
	X(Name, name)																\
	X(Object, object)															\
	X(RootObject, rootObject)													\
	X(DefinitionModel, definitionModel)										\
	X(UrlIsAssetBrowser, urlIsAssetBrowser)									\
	X(UrlDialogTitle, urlDialogTitle)											\
	X(UrlDialogDefaultFolder, urlDialogDefaultFolder)							\
	X(UrlDialogNameFilters, urlDialogNameFilters)								\
	X(UrlDialogSelectedNameFilter, urlDialogSelectedNameFilter)				\
	X(UrlDialogModality, urlDialogModality)									\
	/* Asset Item Extension */													\
	X(StatusIcon, statusIcon)													\
	X(TypeIcon, typeIcon)														\
	X(Size, size)																\
	X(CreatedTime, createdTime)												\
	X(ModifiedTime, modifiedTime)												\
	X(AccessedTime, accessedTime)												\
	X(IsDirectory, isDirectory)												\
	X(IsReadOnly, isReadOnly)													\
	X(IsCompressed, isCompressed)												\
	/* Thumbnail Extension */													\
	X(Thumbnail, thumbnail)													\
	X(headerText, headerText)													\
	X(footerText, footerText)													\
	/*Buttons Extensions*/														\
    X(buttonsDefinition, buttonsDefinition)									\
    X(buttonIcon, buttonIcon)													\
    X(buttonEnabled, buttonEnabled)											\
    X(buttonClicked, buttonClicked)

#define X( ROLE, ROLENAME ) 													\
	class ROLE##Role															\
	{																			\
	public:																		\
		static const char * roleName_;											\
		static ::wgt::ItemRole::Id roleId_;											\
	};
	ITEM_ROLES
#undef X
} // end namespace wgt
#endif //I_ITEM_ROLE_HPP
