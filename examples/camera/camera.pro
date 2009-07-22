TEMPLATE = app

DEFINES += VIDEOSERVICES
QT += multimedia

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints

LIBS += -L$$OUT_PWD/../../lib -lQtMedia

QMAKE_RPATHDIR += $$OUT_PWD/../../lib

DESTDIR = ../../bin

HEADERS = \
    camera.h
  
SOURCES = \
    main.cpp \
    camera.cpp
    
