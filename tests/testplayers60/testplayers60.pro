TEMPLATE = app
TARGET = testplayers60
QT += network \
      xml

INCLUDEPATH += ../../src/multimedia ../../src/multimedia/audio

include(../../common.pri)
CONFIG += mobility
MOBILITY = multimedia

HEADERS = \
    testplayers60.h \
    playercontrols.h \
    playlistmodel.h \
    videowidget.h
SOURCES = main.cpp \
    testplayers60.cpp \
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
