TEMPLATE = app
TARGET = camera

INCLUDEPATH+=../../src/multimedia
include(../examples.pri)

CONFIG += mobility
MOBILITY = multimedia

HEADERS = \
    camera.h \
    imagesettings.h \
    videosettings.h

SOURCES = \
    main.cpp \
    camera.cpp \
    imagesettings.cpp \
    videosettings.cpp

FORMS += \
    camera.ui \
    videosettings.ui \
    imagesettings.ui

symbian {
    TARGET.CAPABILITY = UserEnvironment WriteDeviceData ReadDeviceData
}
