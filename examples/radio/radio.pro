TEMPLATE = app
TARGET = radio

INCLUDEPATH += ../../src/multimedia

include(../examples.pri)
qtAddLibrary(QtMedia)

HEADERS = \
    radio.h
  
SOURCES = \
    main.cpp \
    radio.cpp
    
