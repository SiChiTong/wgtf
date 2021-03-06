CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_async_loader_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )

find_package(QT5 ${QT_FIND_PACKAGE_VERSION})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plg_async_loader_test.cpp
	test_model.cpp
	test_model.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC( plg_async_loader_test resources/plg_async_loader_test )

BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_async_loader_test SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_async_loader_test PRIVATE
	core_generic_plugin
	core_data_model
)

BW_PROJECT_CATEGORY( plg_async_loader_test "Plugins" )
