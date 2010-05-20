TARGET = tst_qmediarecorder_s60
INCLUDEPATH += ../../../../src/multimedia \
    ../../../../include
CONFIG += testcase

SOURCES += tst_qmediarecorder_s60.cpp

include (../../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia

#UserEnvironment WriteDeviceData ReadDeviceData MultimediaDD 
symbian {
        TARGET.CAPABILITY = ALL -TCB
}
