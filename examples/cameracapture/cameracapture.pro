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
FORMS += cameracapture.ui \
    settings.ui
    
symbian:TARGET.CAPABILITY = UserEnvironment
