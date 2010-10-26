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

wince*:symbian {
    deploy.files += 4.wav
    DEPLOYMENT += deploy
    !symbian:DEFINES += SRCDIR=\\\"\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

unix:!mac {
    !contains(QT_CONFIG, pulseaudio) {
        DEFINES += QT_MULTIMEDIA_QMEDIAPLAYER
    }
}
