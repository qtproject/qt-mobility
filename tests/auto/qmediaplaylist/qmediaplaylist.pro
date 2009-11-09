TARGET = tst_qmediaplaylist
INCLUDEPATH += ../../../multimedia
CONFIG += testcase

include (../../../common.pri)

HEADERS += \
    $$SOURCE_DIR/plugins/m3u/qm3uhandler.h

SOURCES += \
    tst_qmediaplaylist.cpp \
    $$QT_MOBILITY_SOURCE_TREE/plugins/m3u/qm3uhandler.cpp

INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/m3u

qtAddLibrary(QtMedia)

