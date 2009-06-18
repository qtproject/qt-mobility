TEMPLATE = lib
TARGET = QtContextFramework
QT = core

include(../common.pri)

DEFINES += QT_BUILD_CFW_LIB QT_MAKEDLL

HEADERS += qpacketprotocol.h qmallocpool.h qvaluespace.h
SOURCES += qpacketprotocol.cpp qmallocpool.cpp qvaluespace.cpp
