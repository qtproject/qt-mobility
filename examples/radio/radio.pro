TEMPLATE = app
CONFIG += example

include (../../common.pri)

qtAddLibrary(QtMedia)

QMAKE_RPATHDIR += $$DESTDIR

HEADERS = \
    radio.h
  
SOURCES = \
    main.cpp \
    radio.cpp
    
