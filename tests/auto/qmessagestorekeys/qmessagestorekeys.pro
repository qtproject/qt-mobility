TEMPLATE = app
TARGET = tst_qmessagestorekeys

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
    tst_qmessagestorekeys.cpp
    
symbian {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    contains(messaging_freestyle_enabled, yes) {
        CONFIG += FREESTYLEMAIL
        DEFINES += FREESTYLEMAILUSED
        DEFINES += FREESTYLEMAILBOXOBSERVERUSED
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

