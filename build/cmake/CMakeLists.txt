# CMakeLists.txt file for BlockModDemo Test User Interface and dependent libraries

project( BlockModProject )

# Require a fairly recent cmake version
cmake_minimum_required( VERSION 2.8.12 )


# -------------------------------------------------------------
# MACRO definitions
# -------------------------------------------------------------

# Macros to hide/show cached variables.
# These two macros can be used to "hide" or "show" in the
# list of cached variables various variables and/or options
# that depend on other options.
# Note that once a variable is modified, it will preserve its
# value (hidding it merely makes it internal)

MACRO(HIDE_VARIABLE var)
  IF(DEFINED ${var})
    SET(${var} "${${var}}" CACHE INTERNAL "")
  ENDIF(DEFINED ${var})
ENDMACRO(HIDE_VARIABLE)

MACRO(SHOW_VARIABLE var type doc default)
  IF(DEFINED ${var})
    SET(${var} "${${var}}" CACHE "${type}" "${doc}" FORCE)
  ELSE(DEFINED ${var})
    SET(${var} "${default}" CACHE "${type}" "${doc}")
  ENDIF(DEFINED ${var})
ENDMACRO(SHOW_VARIABLE)

# -------------------------------------------------------------
# Initial commands
# -------------------------------------------------------------

# Uncomment this to enable detailed make output
set( CMAKE_VERBOSE_MAKEFILE OFF )

# automatically add CMAKE_CURRENT_SOURCE_DIR and CMAKE_CURRENT_BINARY_DIR to the include directories in every processed CMakeLists.txt
set( CMAKE_INCLUDE_CURRENT_DIR ON )

# Hide some more cache variables to keep things tidy
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
MARK_AS_ADVANCED(EXECUTABLE_OUTPUT_PATH LIBRARY_OUTPUT_PATH)

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

# Test for Qt5
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Concurrent REQUIRED)  # needed for qwt6

if( APPLE )
	find_library(IOKIT NAMES IOKit REQUIRED)
	find_library(COREFOUNDATION NAMES CoreFoundation REQUIRED)
	find_library(SECURITY_FRAMEWORK Security REQUIRED)
	find_library(SYSTEM_CONFIGURATION SystemConfiguration REQUIRED)

	set(APPLE_FRAMEWORKS
		${IOKIT}
		${SECURITY_FRAMEWORK}
		${COREFOUNDATION}
		${SYSTEM_CONFIGURATION}
	)
endif( APPLE )

# -------------------------------------------------------------
# subdirectories and subprojects
# -------------------------------------------------------------

# add subdirectories for all builds
add_subdirectory( ../../BlockMod/projects/cmake BlockMod )
add_subdirectory( ../../BlockModDemo/projects/cmake BlockModDemo)
add_subdirectory( ../../BlockModTests/projects/cmake BlockModTests)
