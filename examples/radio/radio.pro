TEMPLATE = app
CONFIG += example

INCLUDEPATH += ../../multimedia ../../multimedia/endpoints
include(../examples.pri)

qtAddLibrary(QtMedia)

QMAKE_RPATHDIR += $$DESTDIR

HEADERS = \
    radio.h
  
SOURCES = \
    main.cpp \
    radio.cpp
    
