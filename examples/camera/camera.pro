TEMPLATE = app
CONFIG += example

QT += multimedia

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints
include (../../common.pri)

LIBS += -lQtMedia

QMAKE_RPATHDIR += $$DESTDIR

HEADERS = \
    camera.h
  
SOURCES = \
    main.cpp \
    camera.cpp
    
