# ----------------------------------------------------
# Project for SerializationTest
# remember to set DYLD_FALLBACK_LIBRARY_PATH on MacOSX
# ----------------------------------------------------

TARGET = ShowNetworkTest
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

SOURCES += \
	../../src/ShowNetworkTest.cpp


