CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_custom_ui_themes )

INCLUDE( WGToolsCoreProject )

FIND_PACKAGE( QT5 ${QT_FIND_PACKAGE_VERSION})


SET( ALL_SRCS
	custom_ui_themes_dialog.hpp
	custom_ui_themes_dialog.cpp
	plg_custom_ui_themes.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_QT_ADD_QRC( WGCustomUIThemes resources/WGCustomUIThemes )
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
