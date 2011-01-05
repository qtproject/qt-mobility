TARGET = tst_qcamera
INCLUDEPATH += \
        ../../../src/multimedia \
        ../../../src/multimedia/video
CONFIG += testcase

SOURCES += tst_qcamera.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

