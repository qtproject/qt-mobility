TEMPLATE = app
TARGET = player
QT += network \
      xml

INCLUDEPATH += ../../src/multimedia ../../src/multimedia/audio

include(../../examples/examples.pri)
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
    TARGET.CAPABILITY = UserEnvironment WriteDeviceData ReadDeviceData NetworkServices SwEvent
    HEADERS += mediakeysobserver.h
    SOURCES += mediakeysobserver.cpp
    LIBS += -lremconinterfacebase \
            -lremconcoreapi
 }
