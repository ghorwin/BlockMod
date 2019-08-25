# CMakeLists.txt file for BlockModTests

project( BlockModTests )

include_directories(
	${PROJECT_SOURCE_DIR}/../../src		# needed so that ui-generated header files find our own headers
	${PROJECT_SOURCE_DIR}/../../../BlockMod/src
	${Qt5Widgets_INCLUDE_DIRS}
	${Qt5PrintSupport_INCLUDE_DIRS}
	${Qt5Network_INCLUDE_DIRS}
	${Qt5Svg_INCLUDE_DIRS}
	${Qt5Xml_INCLUDE_DIRS}
)

# generate list of all required QT_LIBRARIES
set ( QT_LIBRARIES
	Qt5::Widgets
	Qt5::PrintSupport
	Qt5::Svg
	Qt5::Xml
	Qt5::Network
	Qt5::Concurrent # needed for qwt6
)


# build application executable for the different platforms
if( WIN32 )
	add_executable( SerializationTest WIN32 ${PROJECT_SOURCE_DIR}/../../src/SerializationTest.cpp )

	set(WIN_FRAMEWORKS iphlpapi EmfEngine gdiplus gdi32 user32 ole32)

endif( WIN32 )

if( UNIX )
	add_executable( SerializationTest ${PROJECT_SOURCE_DIR}/../../src/SerializationTest.cpp )
endif( UNIX )

target_link_libraries( SerializationTest
	BlockMod
	${QT_LIBRARIES}
	${APPLE_FRAMEWORKS}
	${WIN_FRAMEWORKS}
)
