# CMakeLists.txt file for BlockMod library

project( BlockMod )

set(BlockMod_VERSION_MAJOR 1)
set(BlockMod_VERSION_MINOR 1)
set(BlockMod_VERSION_PATCH 0)
set(BlockMod_VERSION ${BlockMod_VERSION_MAJOR}.${BlockMod_VERSION_MINOR}.${BlockMod_VERSION_PATCH})

cmake_minimum_required( VERSION 2.8...3.10 )

# enable @rpath on MacOS
cmake_policy(SET CMP0042 NEW)
# set linker search policy new
cmake_policy(SET CMP0003 NEW)
# Policy CMP0020: Automatically link Qt executables to qtmain target on Windows.
cmake_policy(SET CMP0020 NEW)

# allow either shared or static lib building, shared library is the default
# Note: when using BlockMod within top-level project, move this line to top-level and comment it out here!
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Set default build type
if (NOT CMAKE_BUILD_TYPE)
	set( CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
		"Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

if (MSVC)

	# add VC compiler-specific flags (NONMINMAX define and disable "unsafe" warnings)
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D\"NOMINMAX\" /wd4996" )

else (MSVC)

	# on Unix we want really detailed warnings
	ADD_DEFINITIONS( -Wall -fPIC)
	# when building with gcc/icc add compile flag
	if (APPLE)
		#message( STATUS "Mac compiler ID = ${CMAKE_C_COMPILER_ID}" )
		if ( CMAKE_C_COMPILER_ID MATCHES "GNU")
			set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
		else ( CMAKE_C_COMPILER_ID MATCHES "GNU")
			set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++" )
		endif ( CMAKE_C_COMPILER_ID MATCHES "GNU")
	else (APPLE)
		set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -std=gnu++11" )
	endif (APPLE)

endif (MSVC)

# -------------------------------------------------------------
# Packages
# -------------------------------------------------------------

# automatically add CMAKE_CURRENT_SOURCE_DIR and CMAKE_CURRENT_BINARY_DIR to the include directories in every processed CMakeLists.txt
set(CMAKE_INCLUDE_CURRENT_DIR ON)

	# Test for Qt5
	find_package(Qt5Widgets REQUIRED)

message("*** Building with Qt5, Version ${Qt5Widgets_VERSION} ***")



# collect a list of all source files
file( GLOB LIB_SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp )

# collect a list of all header files (to be used in MOC compiler)
file( GLOB LIB_HDRS ${PROJECT_SOURCE_DIR}/src/*.h )


include_directories(
	${PROJECT_SOURCE_DIR}/src			# needed so that ui-generated header files find our own headers
	${Qt5Widgets_INCLUDE_DIRS}
)

qt5_wrap_cpp( LIB_MOC_SRCS ${LIB_HDRS} )

# add build target for library
add_library( ${PROJECT_NAME}
	${LIB_SRCS}
	${LIB_MOC_SRCS}
)

# set version numbers when building as shared library
set_target_properties(${PROJECT_NAME} PROPERTIES
						VERSION ${BlockMod_VERSION}
						SOVERSION ${BlockMod_VERSION_MAJOR}
					 )

# state that library need PIC when the default is shared libraries
set_target_properties(${PROJECT_NAME} PROPERTIES
						POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
					 )
