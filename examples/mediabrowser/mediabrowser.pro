TEMPLATE = app
TARGET = mediabrowser

include(../mobility_examples.pri)
CONFIG += mobility
MOBILITY = gallery

INCLUDEPATH += ../../src/gallery
DEPENDPATH += ../../src/gallery

HEADERS = \
    albumdelegate.h \
    albumview.h \
    artistdelegate.h \
    artistview.h \
    browser.h \
    coverartmodel.h \
    galleryview.h \
    photodelegate.h \
    photoview.h \
    songview.h \
    thumbnailcache.h \
    thumbnailmodel.h

SOURCES = \
    albumdelegate.cpp \
    albumview.cpp \
    artistdelegate.cpp \
    artistview.cpp \
    browser.cpp \
    coverartmodel.cpp \
    galleryview.cpp \
    main.cpp \
    photodelegate.cpp \
    photoview.cpp \
    songview.cpp \
    thumbnailcache.cpp \
    thumbnailmodel.cpp

symbian: {
    load(data_caging_paths)
}

contains(MEEGO_EDITION,harmattan) {

    DEFINES += Q_WS_MAEMO_6

    icon.path = /usr/share/icons/hicolor/80x80/apps
    icon.files += mediabrowser.png

    desktop.path = /usr/share/applications
    desktop.files += mediabrowser.desktop

    target.path = /opt/mediabrowser/bin
    INSTALLS += target icon desktop
}
