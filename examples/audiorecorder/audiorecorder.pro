TEMPLATE = app
CONFIG += example

contains(QT_CONFIG, multimedia): QT += multimedia

INCLUDEPATH += ../../src/multimedia
include(../examples.pri)

qtAddLibrary(QtMedia)

QMAKE_RPATHDIR += $$DESTDIR

HEADERS = \
    audiorecorder.h
  
SOURCES = \
    main.cpp \
    audiorecorder.cpp
    
