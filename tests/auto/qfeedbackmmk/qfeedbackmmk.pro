TARGET = tst_qfeedbackmmk
CONFIG += testcase
INCLUDEPATH += ../../../src/feedback

include (../../../common.pri)

SOURCES += tst_qfeedbackmmk.cpp

wince*|symbian {
    deploy.files = test.wav
    DEPLOYMENT = deploy
    DEFINES += QT_QFEEDBACKMMK_USEAPPLICATIONPATH
} else:maemo* {
    DEFINES += QT_QFEEDBACKMMK_USEAPPLICATIONPATH
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

QT += core
CONFIG += mobility
MOBILITY = feedback

