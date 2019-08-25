# ----------------------------------------------------
# Project for SerializationTest
# remember to set DYLD_FALLBACK_LIBRARY_PATH on MacOSX
# ----------------------------------------------------

TARGET = SerializationTest


QT += widgets svg network xml printsupport

TEMPLATE = app

CONFIG(debug, debug|release) {
	OBJECTS_DIR = debug
	DESTDIR = ../../../bin/debug
}
else {
	OBJECTS_DIR = release
	DESTDIR = ../../../bin/release
}

MOC_DIR = moc
UI_DIR = ui

INCLUDEPATH = \
	../../src \
	../../../BlockMod/src

LIBS += -L../../../lib \
	-lBlockMod

SOURCES += \
	../../src/serialization_main.cpp


