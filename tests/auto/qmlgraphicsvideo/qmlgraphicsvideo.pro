TARGET = tst_qmlgraphicsvideo
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

QT += multimedia declarative

include (../../../common.pri)

SOURCES += tst_qmlgraphicsvideo.cpp

CONFIG += mobility
MOBILITY = multimedia
