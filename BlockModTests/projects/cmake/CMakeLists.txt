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

# collect a list of all source files
file( GLOB APP_SRCS ${PROJECT_SOURCE_DIR}/../../src/*.cpp )

# collect a list of all header files (to be used in MOC compiler)
file( GLOB APP_HDRS ${PROJECT_SOURCE_DIR}/../../src/*.h )

# collect a list of all form files
file( GLOB APP_UIS ${PROJECT_SOURCE_DIR}/../../src/*.ui )

# The Qt resource files (*.qrc)
#set( APP_QRCS ${PROJECT_SOURCE_DIR}/../../resources/BlockModTests.qrc )

# set rc file
#set ( APP_WIN_RC ${PROJECT_SOURCE_DIR}/../../resources/BlockModTests.rc )

# run uic and moc compilers, add resources

qt5_add_resources( APP_RC_SRCS ${APP_QRCS} )
qt5_wrap_ui( APP_UI_SRCS ${APP_UIS} )
qt5_wrap_cpp( APP_MOC_SRCS ${APP_HDRS} )

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
	add_executable( ${PROJECT_NAME} WIN32
					 ${APP_SRCS} ${APP_MOC_SRCS} ${APP_RC_SRCS} ${APP_UI_SRCS} ${APP_WIN_RC}
	)

	set(WIN_FRAMEWORKS iphlpapi EmfEngine gdiplus gdi32 user32 ole32)

endif( WIN32 )

if( UNIX )
	if( APPLE )
		add_executable( ${PROJECT_NAME} MACOSX_BUNDLE
						 ${APP_SRCS} ${APP_MOC_SRCS} ${APP_RC_SRCS} ${APP_UI_SRCS}
						 ${PROJECT_SOURCE_DIR}/../../resources/gfx/BlockModTests.icns
		)
	else( APPLE )
		add_executable( ${PROJECT_NAME}
						 ${APP_SRCS} ${APP_MOC_SRCS} ${APP_RC_SRCS} ${APP_UI_SRCS}
		)
	endif( APPLE )
endif( UNIX )

target_link_libraries( ${PROJECT_NAME}
	BlockMod
	${QT_LIBRARIES}
	${APPLE_FRAMEWORKS}
	${WIN_FRAMEWORKS}
)
