TARGET = tst_qmediaplaylist
INCLUDEPATH += ../../../src/multimedia
CONFIG += testcase

include (../../../common.pri)

HEADERS += \
    $$SOURCE_DIR/plugins/multimedia/m3u/qm3uhandler.h

SOURCES += \
    tst_qmediaplaylist.cpp \
    $$QT_MOBILITY_SOURCE_TREE/plugins/multimedia/m3u/qm3uhandler.cpp

INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/multimedia/m3u

CONFIG += mobility
MOBILITY = multimedia

