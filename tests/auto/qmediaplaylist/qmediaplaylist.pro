TARGET = tst_qmediaplaylist

CONFIG += testcase

include (../../../common.pri)

HEADERS += \
    $$SOURCE_DIR/plugins/m3u/qm3uhandler.h

SOURCES += \
    tst_qmediaplaylist.cpp \
    $$SOURCE_DIR/plugins/m3u/qm3uhandler.cpp

INCLUDEPATH += $$SOURCE_DIR/plugins/m3u

LIBS += -lQtMedia

