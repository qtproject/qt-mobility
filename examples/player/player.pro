TEMPLATE = app
TARGET = player
QT += network \
      xml

INCLUDEPATH += ../../src/multimedia

include(../examples.pri)
CONFIG += mobility
MOBILITY = multimedia

contains(QT_CONFIG,multimedia) {
    QT += multimedia
}


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
