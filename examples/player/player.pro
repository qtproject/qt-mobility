TEMPLATE = app

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints

LIBS += -L$$OUT_PWD/../../lib -lQtMedia

QMAKE_RPATHDIR += $$OUT_PWD/../../lib

DESTDIR = ../../bin

HEADERS = \
    player.h \
    playlistmodel.h
  
SOURCES = \
    main.cpp \
    player.cpp \
    playlistmodel.cpp
    
