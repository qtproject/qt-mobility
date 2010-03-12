TARGET = tst_qmediaplaylist_s60
INCLUDEPATH += ../../../../include \
    ../../../../src/multimedia
CONFIG += testcase

include (../../../../common.pri)

HEADERS += \
    $$SOURCE_DIR/plugins/multimedia/m3u/qm3uhandler.h

SOURCES += \
    tst_qmediaplaylist_s60.cpp \
    $$QT_MOBILITY_SOURCE_TREE/plugins/multimedia/m3u/qm3uhandler.cpp

INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/multimedia/m3u

CONFIG += mobility
MOBILITY = multimedia

TARGET.CAPABILITY = ALL -TCB
