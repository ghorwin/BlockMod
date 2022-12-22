# ----------------------------------------------------
# Project for BlockMod Toolkit Library
# ----------------------------------------------------

TARGET = BlockMod
TEMPLATE = lib

# common project configurations, source this file after TEMPLATE was specified
include( BlockMod.pri )

QT += core gui network xml widgets

# finally we setup our custom library specfic things
# like version number etc.
unix|mac {
	VER_MAJ = 1
	VER_MIN = 1
	VER_PAT = 0
	VERSION = $${VER_MAJ}.$${VER_MIN}.$${VER_PAT}
}

INCLUDEPATH = \
	src

HEADERS += \
	src/BM_ConnectorSegmentItem.h \
	src/BM_Globals.h \
	src/BM_SocketItem.h \
	src/BM_ZoomMeshGraphicsView.h \
	src/BM_Block.h \
	src/BM_Connector.h \
	src/BM_Socket.h \
	src/BM_Network.h \
	src/BM_XMLHelpers.h \
	src/BM_SceneManager.h \
	src/BM_BlockItem.h
SOURCES += \
	src/BM_ConnectorSegmentItem.cpp \
	src/BM_Globals.cpp \
	src/BM_SocketItem.cpp \
	src/BM_ZoomMeshGraphicsView.cpp \
	src/BM_Network.cpp \
	src/BM_Block.cpp \
	src/BM_Socket.cpp \
	src/BM_XMLHelpers.cpp \
	src/BM_Connector.cpp \
	src/BM_SceneManager.cpp \
	src/BM_BlockItem.cpp
FORMS +=

DISTFILES += \
	doc/BlockMod-mainpage.h

