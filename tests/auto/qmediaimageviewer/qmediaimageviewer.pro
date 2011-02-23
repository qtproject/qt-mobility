TARGET = tst_qmediaimageviewer
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/video

include (../../../common.pri)

SOURCES += tst_qmediaimageviewer.cpp

RESOURCES += \
        images.qrc

QT += network
CONFIG += mobility
MOBILITY = multimedia

!contains(QT_CONFIG, no-jpeg):DEFINES += QTEST_HAVE_JPEG

wince* {
    !contains(QT_CONFIG, no-jpeg): DEPLOYMENT_PLUGIN += qjpeg
}
