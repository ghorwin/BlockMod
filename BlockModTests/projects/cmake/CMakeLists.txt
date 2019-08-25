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

# collect a list of all source files within the examples directory
file( GLOB IBK_EXAMPLE_SRCS ${PROJECT_SOURCE_DIR}/../../src/*.cpp )

foreach( SRC ${IBK_EXAMPLE_SRCS} )
	get_filename_component( EXAMPLE_FILENAME ${SRC} NAME_WE )
	if( WIN32 )
		add_executable( ${EXAMPLE_FILENAME}	WIN32 ${SRC} )
	endif( WIN32 )

	if( UNIX )
		add_executable( ${EXAMPLE_FILENAME}	${SRC} )
	endif( UNIX )
	
	# link against the IBK library
	target_link_libraries( ${EXAMPLE_FILENAME}
		BlockMod
		${QT_LIBRARIES}
		${APPLE_FRAMEWORKS}
		${WIN_FRAMEWORKS}
	)
endforeach( SRC )

