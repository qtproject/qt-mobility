TARGET = tst_qcamera_s60
INCLUDEPATH += ../../../../src/multimedia \
    ../../../../include
CONFIG += testcase

SOURCES += tst_qcamera_s60.cpp

include (../../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia


symbian {
        TARGET.CAPABILITY = UserEnvironment WriteDeviceData MultimediaDD ReadDeviceData
}
