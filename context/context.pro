TEMPLATE = lib
TARGET = QtContextFramework
QT = core network gui

include(../common.pri)

DEFINES += QT_BUILD_CFW_LIB QT_MAKEDLL

HEADERS += qcontextglobal.h qpacketprotocol.h qmallocpool.h qsystemlock.h qvaluespace.h
SOURCES += applayer.cpp qpacketprotocol.cpp qmallocpool.cpp qsystemlock.cpp qvaluespace.cpp
