TARGET = tst_qvideowidget
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

QT += multimedia

include (../../../common.pri)

SOURCES += tst_qvideowidget.cpp

CONFIG += mobility
MOBILITY = multimedia
