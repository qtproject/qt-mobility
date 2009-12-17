TEMPLATE = app
TARGET = player
QT += network \
      xml

INCLUDEPATH += ../../src/multimedia

include(../examples.pri)
CONFIG += mobility
qtAddLibrary(QtMedia)

HEADERS = \
    player.h \
    playercontrols.h \
    videowidget.h
SOURCES = main.cpp \
    player.cpp \
    playercontrols.cpp \
    videowidget.cpp
    
include(mediakeysobserver.pri)
           
symbian:TARGET.CAPABILITY = ALL -TCB
