# CMakeLists.txt file for BlockMod library

project( BlockMod )

# collect a list of all source files
file( GLOB LIB_SRCS ${PROJECT_SOURCE_DIR}/../../src/*.cpp )

# collect a list of all header files (to be used in MOC compiler)
file( GLOB LIB_HDRS ${PROJECT_SOURCE_DIR}/../../src/*.h )

# collect a list of all form files
file( GLOB LIB_UIS ${PROJECT_SOURCE_DIR}/../../src/*.ui )

# set resource files
set( LIB_QRCS ${PROJECT_SOURCE_DIR}/../../resources/BlockMod.qrc )

get_target_property(QtCore_location Qt5::Core LOCATION)
get_filename_component(Qt5BinaryDir ${QtCore_location} DIRECTORY)

include_directories(
	${PROJECT_SOURCE_DIR}/../../src			# needed so that ui-generated header files find our own headers
	${PROJECT_SOURCE_DIR}/../../../IBK/src
	${Qt5Widgets_INCLUDE_DIRS}
	${Qt5PrintSupport_INCLUDE_DIRS}
	${Qt5Network_INCLUDE_DIRS}
	${Qt5Svg_INCLUDE_DIRS}
	${Qt5Xml_INCLUDE_DIRS}
	${Qt5BinaryDir}/../../Src/qtbase/include/QtZlib # for using Qt's own zlib 
)

qt5_add_resources( LIB_RC_SRCS ${LIB_QRCS} )
qt5_wrap_ui( LIB_UI_SRCS ${LIB_UIS} )
qt5_wrap_cpp( LIB_MOC_SRCS ${LIB_HDRS} )

# add build target for library
add_library( ${PROJECT_NAME} STATIC
	${LIB_SRCS}
	${LIB_MOC_SRCS}
	${LIB_UI_SRCS}
	${LIB_RC_SRCS}
)
