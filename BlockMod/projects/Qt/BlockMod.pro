# ----------------------------------------------------
# Project for BlockMod Toolkit Library
# ----------------------------------------------------

TARGET = BlockMod
TEMPLATE = lib

# this pri must be sourced from all our libraries,
# it contains all functions defined for casual libraries
include( ../../../IBK/projects/Qt/IBK.pri )

QT += core gui network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH = \
	../../src

RESOURCES += ../../resources/BlockMod.qrc

HEADERS += \
	../../src/BM_ZoomMeshGraphicsView.h \
	../../src/BM_Block.h \
	../../src/BM_Connector.h \
	../../src/BM_Socket.h \
	../../src/BM_Network.h \
	../../src/BM_Entity.h \
	../../src/BM_XMLHelpers.h \
	../../src/BM_SceneManager.h \
	../../src/BM_BlockItem.h
SOURCES += \
	../../src/BM_ZoomMeshGraphicsView.cpp \
	../../src/BM_Network.cpp \
	../../src/BM_Block.cpp \
	../../src/BM_Entity.cpp \
	../../src/BM_Socket.cpp \
	../../src/BM_XMLHelpers.cpp \
	../../src/BM_Connector.cpp \
	../../src/BM_SceneManager.cpp \
	../../src/BM_BlockItem.cpp
FORMS +=

