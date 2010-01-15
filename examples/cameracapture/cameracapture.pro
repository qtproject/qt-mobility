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
FORMS += cameracapture.ui \
    settings_s60.ui
TARGET.CAPABILITY = UserEnvironment WriteDeviceData
} else {
FORMS += cameracapture.ui \
    settings.ui
}
