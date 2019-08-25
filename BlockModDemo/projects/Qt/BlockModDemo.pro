# ----------------------------------------------------
# Project for BlockModDemo Test User Interface
# remember to set DYLD_FALLBACK_LIBRARY_PATH on MacOSX
# ----------------------------------------------------

TARGET = BlockModDemo
TEMPLATE = app

# common project configurations, source this file after TEMPLATE was specified
include( ../../../BlockMod/projects/Qt/BlockMod.pri )

QT += widgets svg network xml printsupport

INCLUDEPATH = \
	../../src \
	../../../BlockMod/src

DEPENDPATH = $${INCLUDEPATH}

LIBS += -L../../../lib$${DIR_PREFIX} \
	-lBlockMod

RESOURCES += ../../resources/BlockModDemo.qrc

HEADERS += \
	../../src/BlockModAppMainWindow.h \
	../../src/BlockModAppDebugApplication.h
SOURCES += \
	../../src/main.cpp \
	../../src/BlockModAppMainWindow.cpp \
	../../src/BlockModAppDebugApplication.cpp
FORMS += \
	../../src/BlockModAppMainWindow.ui

