TEMPLATE = app
CONFIG += example

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints
include (../../common.pri)

qtAddLibrary(QtMedia)

QMAKE_RPATHDIR += $$DESTDIR

HEADERS = \
    radio.h
  
SOURCES = \
    main.cpp \
    radio.cpp
    
