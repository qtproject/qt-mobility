TEMPLATE = app
CONFIG += example

INCLUDEPATH += ../../src/multimedia
include(../examples.pri)

qtAddLibrary(QtMedia)

QMAKE_RPATHDIR += $$DESTDIR

HEADERS = \
    radio.h
  
SOURCES = \
    main.cpp \
    radio.cpp
    
