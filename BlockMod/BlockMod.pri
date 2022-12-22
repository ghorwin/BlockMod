# Common build configurations for BlockMod library and test/demo applications

# This option enables extensive runtime checks.
#OPTIONS += sanitize_checks


CONFIG			+= silent
CONFIG			-= depend_includepath
CONFIG			+= c++11

CONFIG(release, debug|release) {
#	message( "Setting NDEBUG define" )
	DEFINES += NDEBUG
}

linux-g++ | linux-g++-64 | macx {

	# our code doesn't check errno after calling math functions
	# so it is perfectly safe to disable it in favor of better performance
	# use *= to uniquely assign option
	QMAKE_CXXFLAGS   *= -fno-math-errno
}

contains( OPTIONS, sanitize_checks ) {

	CONFIG(debug, debug|release) {
		CONFIG += sanitizer
		CONFIG += sanitize_address
		CONFIG += sanitize_undefined
	}

	linux-g++ | linux-g++-64 | macx {
		QMAKE_CXXFLAGS_DEBUG   *= -fsanitize=address -fno-omit-frame-pointer
	}
}


#
# *** Applications ***
#
# This section contains all application specific settings. It can be enabled
# by setting 'app' the TEMPLATE environment variable.
# It defines DESTDIR, OBJECTS_DIR
# Reset locally in when required.
equals(TEMPLATE,app) {

	CONFIG(debug, debug|release) {
		OBJECTS_DIR = debug
		DESTDIR = ../bin/debug
	}
	else {
		OBJECTS_DIR = release
		DESTDIR = ../bin/release
	}

	MOC_DIR = moc
	UI_DIR = ui

	win32-msvc* {
		# disable warning for unsafe functions if using MS compiler
		QMAKE_CXXFLAGS += /wd4996
		QMAKE_CFLAGS += /wd4996
		DEFINES += NOMINMAX
		DEFINES += _CRT_SECURE_NO_WARNINGS

		# In Debug mode add warnings for access to uninitialized variables
		# and out-of-bounds access for static arrays (and vectors)
		CONFIG(debug, debug|release) {
			QMAKE_CXXFLAGS += /GS /RTC1
		}
	}
}



#
# *** Libraries ***
#
# This section contains all library specific settings. It can be enabled
# by setting 'lib' the TEMPLATE environment variable.
# It defines DESTDIR, OBJECTS_DIR, DLLDESTDIR and sets shared in CONFIG
# variable to all libraries. Reset locally in when required.
equals(TEMPLATE,lib) {

#	message(Setting up ordinary library support.)
	QT -=	core gui

	CONFIG += warn_on

	# set this even in case of no Qt library compilation to get mocs/uis organized in subdirs
	MOC_DIR = moc
	UI_DIR = ui

	# using of shared libs only for non MC compiler
	# MS compiler needs explicite export statements in case of shared libs
	win32-msvc* {
		CONFIG += static
		DEFINES += NOMINMAX
		# disable warning for unsafe functions if using MS compiler
		QMAKE_CXXFLAGS += /wd4996
		QMAKE_CFLAGS += /wd4996
		DEFINES += _CRT_SECURE_NO_WARNINGS
		# In Debug mode add warnings for access to uninitialized variables
		# and out-of-bounds access for static arrays (and vectors)
		CONFIG(debug, debug|release) {
			QMAKE_CXXFLAGS += /GS /RTC1
		}
	}
	else {
		# on Unix/MacOS we always build our libraries as dynamic libs
		CONFIG += shared
	}

	DESTDIR = ../lib
	LIBS += -L../lib

	CONFIG(debug, debug|release) {
		OBJECTS_DIR = debug
		windows {
			contains( OPTIONS, top_level_libs ) {
				DLLDESTDIR = ../bin/debug
			}
			else {
				DLLDESTDIR = ../../bin/debug
			}
		}
	}
	else {
		OBJECTS_DIR = release
		windows {
			contains( OPTIONS, top_level_libs ) {
				DLLDESTDIR = ../bin/release
			}
			else {
				DLLDESTDIR = ../../bin/release
			}
		}
	}


}
