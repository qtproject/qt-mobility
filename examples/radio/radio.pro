TEMPLATE = app
CONFIG += example

include (../../common.pri)

LIBS += -lQtMedia

QMAKE_RPATHDIR += $$DESTDIR

HEADERS = \
    radio.h
  
SOURCES = \
    main.cpp \
    radio.cpp
    
