TARGET = tst_qmediaplayerbackend
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

include (../../../common.pri)

symbian*: {
    PLAYLIST_TESTDATA.sources = testdata/*
    PLAYLIST_TESTDATA.path = testdata
    DEPLOYMENT += PLAYLIST_TESTDATA
}

wince* {
    DEFINES+= TESTDATA_DIR=\\\"./\\\"
}else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

SOURCES += \
    tst_qmediaplayerbackend.cpp

CONFIG += mobility
MOBILITY = multimedia

