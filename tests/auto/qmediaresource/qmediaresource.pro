TARGET = tst_qmediaresource
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia

include (../../../common.pri)

QT += network

SOURCES += tst_qmediaresource.cpp

CONFIG += mobility
MOBILITY = multimedia

