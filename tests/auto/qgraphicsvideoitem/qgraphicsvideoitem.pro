TARGET = tst_qgraphicsvideoitem
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/video
CONFIG += testcase

SOURCES += tst_qgraphicsvideoitem.cpp

include (../../../common.pri)

symbian: TARGET.CAPABILITY = ReadDeviceData WriteDeviceData


CONFIG += mobility
MOBILITY = multimedia

