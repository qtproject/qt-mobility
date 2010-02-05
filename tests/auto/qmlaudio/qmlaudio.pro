TARGET = tst_qmlaudio
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

QT += multimedia declarative

include (../../../common.pri)

SOURCES += tst_qmlaudio.cpp

CONFIG += mobility
MOBILITY = multimedia
