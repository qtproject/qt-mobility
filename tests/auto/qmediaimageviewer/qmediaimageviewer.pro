TARGET = tst_qmediaimageviewer
CONFIG += testcase
INCLUDEPATH += ../../../src/multimedia

include (../../../common.pri)

SOURCES += tst_qmediaimageviewer.cpp

QT += network multimedia
CONFIG += mobility
MOBILITY = multimedia

!contains(QT_CONFIG, no-jpeg):DEFINES += QTEST_HAVE_JPEG

wince*|symbian {
    addFiles.sources = images/*
    addFiles.path = tst_qmediaimageviewer_images
    DEPLOYMENT += addFiles
}

wince* {
    DEFINES += TESTDATA_DIR=\\\"./tst_qmediaimageviewer_images\\\"
    !contains(QT_CONFIG, no-jpeg): DEPLOYMENT_PLUGIN += qjpeg
} else: !symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/images\\\"
}
