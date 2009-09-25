TEMPLATE = app
CONFIG += example

include (../../common.pri)

LIBS += -lQtMedia

QMAKE_RPATHDIR += $$DESTDIR

HEADERS = \
    audiorecorder.h
  
SOURCES = \
    main.cpp \
    audiorecorder.cpp
    
