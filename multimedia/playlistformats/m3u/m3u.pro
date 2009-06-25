TARGET  = qm3u
CONFIG += qt plugin static
TEMPLATE = lib

INCLUDEPATH += ../..

HEADERS += qm3uhandler.h
SOURCES += main.cpp \
           qm3uhandler.cpp

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/playlistformats
target.path += $$[QT_INSTALL_PLUGINS]/playlistformats
INSTALLS += target

LIBS += ../../libqmultimedia.a
