TEMPLATE = app

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints

LIBS += -L$$OUT_PWD/../../lib -lQtMedia

QMAKE_RPATHDIR += $$OUT_PWD/../../lib

DESTDIR = ../../bin

HEADERS = \
    radio.h
  
SOURCES = \
    main.cpp \
    radio.cpp
    
