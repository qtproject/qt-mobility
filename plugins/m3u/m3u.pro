TEMPLATE = lib
CONFIG += plugin
TARGET = m3u

PLUGIN_SUBDIR = playlistformats

include (../../common.pri)

LIBS += -lQtMedia

DEPENDPATH += .
INCLUDEPATH += . \
    ../../multimedia

HEADERS += qm3uhandler.h
SOURCES += main.cpp \
           qm3uhandler.cpp

symbian: {
# EPOCALLOWDLLDATA have to set true because Qt macros has initialised global data
TARGET.EPOCALLOWDLLDATA=1
}
