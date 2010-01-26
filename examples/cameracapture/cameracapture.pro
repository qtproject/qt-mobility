TEMPLATE = app
TARGET = cameracapture

INCLUDEPATH+=../../src/multimedia
include(../examples.pri)

CONFIG += mobility
MOBILITY = multimedia

HEADERS = cameracapture.h \
    settings.h
SOURCES = main.cpp \
    cameracapture.cpp \
    settings.cpp

symbian: {
    TARGET.CAPABILITY = UserEnvironment WriteDeviceData ReadDeviceData
    FORMS += cameracapture.ui \
        settings_s60.ui
    } else {
    FORMS += cameracapture.ui \
        settings.ui
}
