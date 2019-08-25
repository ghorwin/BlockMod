# Common build configurations for BlockMod library and test/demo applications

# This option enables extensive runtime checks.
#OPTIONS += sanitize_checks


CONFIG			+= silent
CONFIG			-= depend_includepath

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


# check if 32 or 64 bit version and set prefix variable for using in output paths
greaterThan(QT_MAJOR_VERSION, 4) {
	contains(QT_ARCH, i386): {
		DIR_PREFIX =
	} else {
		DIR_PREFIX = _x64
	}
} else {
	DIR_PREFIX =
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
		OBJECTS_DIR = debug$${DIR_PREFIX}
		DESTDIR = ../../../bin/debug$${DIR_PREFIX}
	}
	else {
		OBJECTS_DIR = release$${DIR_PREFIX}
		DESTDIR = ../../../bin/release$${DIR_PREFIX}
	}

	MOC_DIR = moc
	UI_DIR = ui

	win32-msvc* {
		QMAKE_CXXFLAGS += /wd4996
		QMAKE_CFLAGS += /wd4996
		DEFINES += _CRT_SECURE_NO_WARNINGS
	}
	else {
		QMAKE_CXXFLAGS += -std=c++11
	}

	QMAKE_LIBDIR += ../../../lib$${DIR_PREFIX}
	LIBS += -L../../../lib$${DIR_PREFIX}

	win32:LIBS += -liphlpapi
	win32:LIBS += -lshell32
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
		DEFINES += _CRT_SECURE_NO_WARNINGS
		CONFIG(debug, debug|release) {
			QMAKE_CXXFLAGS += /GS /RTC1
		}
	}
	else {
		CONFIG += shared
	}

	# disable warning for unsafe functions if using MS compiler
	win32-msvc* {
		QMAKE_CXXFLAGS += /wd4996
		QMAKE_CFLAGS += /wd4996
	}
	else {
		QMAKE_CXXFLAGS += -std=c++11
	}

	DESTDIR = ../../../lib$${DIR_PREFIX}
	LIBS += -L../../../lib$${DIR_PREFIX}

	CONFIG(debug, debug|release) {
		OBJECTS_DIR = debug$${DIR_PREFIX}
		windows {
			contains( OPTIONS, top_level_libs ) {
				DLLDESTDIR = ../../../bin/debug$${DIR_PREFIX}
			}
			else {
				DLLDESTDIR = ../../../../bin/debug$${DIR_PREFIX}
			}
		}
	}
	else {
		OBJECTS_DIR = release$${DIR_PREFIX}
		windows {
			contains( OPTIONS, top_level_libs ) {
				DLLDESTDIR = ../../../bin/release$${DIR_PREFIX}
			}
			else {
				DLLDESTDIR = ../../../../bin/release$${DIR_PREFIX}
			}
		}
	}

	win32:LIBS += -lshell32
	win32:LIBS += -liphlpapi

}
