TEMPLATE = subdirs

CONFIG += ordered

DEFINES += QT_NO_VIDEOSURFACE
DEFINES += QT_NO_AUDIODEVICE

SUBDIRS = \
    multimedia \
    examples \
    plugins
