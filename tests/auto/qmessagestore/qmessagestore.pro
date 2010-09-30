TEMPLATE = app
TARGET = tst_qmessagestore

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
    tst_qmessagestore.cpp

symbian {
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

