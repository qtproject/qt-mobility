TEMPLATE = app
TARGET = tst_qmessage

CONFIG += testcase
QT += testlib

include(../../../common.pri)
include(../support/support.pri)

CONFIG += mobility
MOBILITY = messaging
INCLUDEPATH += ../../../src/messaging

wince*|symbian*: {
    addFiles.sources = testdata/*
    addFiles.path = testdata
    DEPLOYMENT += addFiles
}

wince* {
    DEFINES += TEADATA_DIR=\\\".\\\"
} !symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

SOURCES += \
    tst_qmessage.cpp

symbian {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    contains(messaging_freestyle_enabled, yes) {
        CONFIG += FREESTYLEMAIL
        DEFINES += FREESTYLEMAILUSED
    }
    contains(messaging_freestyle_mapi12_enabled, yes) {
        CONFIG += FREESTYLEMAILMAPI12
        DEFINES += FREESTYLEMAILMAPI12USED
    }

    LIBS += -limcm \
            -lcommsdat \
            -lmsgs

    TARGET.CAPABILITY = NetworkServices \
                        LocalServices \
                        ReadUserData \
                        WriteUserData \
                        UserEnvironment \
                        ReadDeviceData \
                        WriteDeviceData
}

