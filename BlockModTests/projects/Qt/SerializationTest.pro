# ----------------------------------------------------
# Project for SerializationTest
# remember to set DYLD_FALLBACK_LIBRARY_PATH on MacOSX
# ----------------------------------------------------

TARGET = SerializationTest
TEMPLATE = app

# common project configurations, source this file after TEMPLATE was specified
include( ../../../BlockMod/projects/Qt/BlockMod.pri )

QT += widgets svg network xml printsupport

INCLUDEPATH = \
	../../src \
	../../../BlockMod/src

LIBS += -L../../../lib$${DIR_PREFIX} \
	-lBlockMod

SOURCES += \
	../../src/SerializationTest.cpp


