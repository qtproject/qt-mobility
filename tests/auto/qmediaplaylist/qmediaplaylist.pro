TARGET = tst_qmediaplaylist
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

HEADERS += \
    $$SOURCE_DIR/plugins/multimedia/m3u/qm3uhandler.h

SOURCES += \
    tst_qmediaplaylist.cpp \
    $$QT_MOBILITY_SOURCE_TREE/plugins/multimedia/m3u/qm3uhandler.cpp

INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/multimedia/m3u

CONFIG += mobility
MOBILITY = multimedia

