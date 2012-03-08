TARGET = tst_qgraphicsvideoitem
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/video
CONFIG += testcase

SOURCES += tst_qgraphicsvideoitem.cpp

include (../../../common.pri)

symbian {
    TARGET.CAPABILITY = ReadDeviceData WriteDeviceData
    contains(QT_CONFIG, egl) {
        LIBS *= -llibegl
    } else {
        DEFINES += QT_NO_EGL
    }
}

CONFIG += mobility
MOBILITY = multimedia

maemo*:CONFIG += insignificant_test
