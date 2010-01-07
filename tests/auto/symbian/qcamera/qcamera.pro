TARGET = tst_qcamera
INCLUDEPATH += ../../../../src/multimedia \
    ../../../../include
CONFIG += testcase

SOURCES += tst_qcamera.cpp

include (../../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

