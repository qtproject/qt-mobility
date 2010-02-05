TEMPLATE = app
TARGET = player

INCLUDEPATH += ../../src/multimedia

include(../examples.pri)
CONFIG += mobility
MOBILITY = multimedia

HEADERS = \
    player.h \
    playercontrols.h \
    playlistmodel.h \
    videowidget.h
SOURCES = main.cpp \
    player.cpp \
    playercontrols.cpp \
    playlistmodel.cpp \
    videowidget.cpp

symbian {
    TARGET.CAPABILITY = UserEnvironment WriteDeviceData ReadDeviceData
    HEADERS += mediakeysobserver.h
    SOURCES += mediakeysobserver.cpp
    LIBS += -lremconinterfacebase \
            -lremconcoreapi
 }
