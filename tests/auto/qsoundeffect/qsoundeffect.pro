TARGET = tst_qsoundeffect
INCLUDEPATH += \
        ../../../src/multimedia \
        ../../../src/multimedia/effects \
        ../../../src/multimedia/audio

DEPENDPATH += ../../src/multimedia
CONFIG += testcase

include (../../../common.pri)

SOURCES += tst_qsoundeffect.cpp

CONFIG += mobility
MOBILITY = multimedia

QT += declarative

wince*|symbian {
    deploy.files = test.wav
    DEPLOYMENT = deploy
    DEFINES += QT_QSOUNDEFFECT_USEAPPLICATIONPATH
} else:maemo* {
    DEFINES += QT_QSOUNDEFFECT_USEAPPLICATIONPATH
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

unix:!mac:!symbian {
    !contains(QT_CONFIG, pulseaudio) {
        DEFINES += QT_MULTIMEDIA_QMEDIAPLAYER
    }
}
