TARGET = tst_qmediacontent
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase
QT += network
include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia
SOURCES += tst_qmediacontent.cpp
