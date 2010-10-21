TEMPLATE = app
TARGET = tst_qmessagestorekeys

CONFIG += testcase
QT += testlib

include(../../../common.pri)
include(../support/support.pri)

CONFIG += mobility
MOBILITY = messaging
INCLUDEPATH += ../../../src/messaging

symbian|wince*|maemo5|win32|mac {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
}

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

        contains(messaging_freestylenm_enabled, yes) { 
            CONFIG += FREESTYLENMAIL
            DEFINES += FREESTYLENMAILUSED
        }
        contains(CONFIG, !FREESTYLENMAIL):contains(messaging_freestyle_enabled, yes) { 
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

