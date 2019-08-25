# ----------------------------------------------------
# Project for BlockModApp Test User Interface
# remember to set DYLD_FALLBACK_LIBRARY_PATH on MacOSX
# ----------------------------------------------------

TARGET = BlockModApp
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += network

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

RESOURCES += ../../resources/BlockModApp.qrc
# TRANSLATIONS += ../../resources/translations/BlockModApp_de.ts

HEADERS += \
	../../src/BlockModAppMainWindow.h \
	../../src/BlockModAppDebugApplication.h
SOURCES += \
	../../src/main.cpp \
	../../src/BlockModAppMainWindow.cpp \
	../../src/BlockModAppDebugApplication.cpp
FORMS += \
	../../src/BlockModAppMainWindow.ui

