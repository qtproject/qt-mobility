TARGET = tst_qmediacontent
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia
SOURCES += tst_qmediacontent.cpp
