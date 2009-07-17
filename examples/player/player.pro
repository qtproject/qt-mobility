TEMPLATE = app

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints

LIBS += -L$$OUT_PWD/../../lib -lQtMedia

QMAKE_RPATHDIR += $$OUT_PWD/../../lib

DESTDIR = ../../bin

HEADERS = \
    player.h \
    playercontrols.h \
    playlistmodel.h
  
SOURCES = \
    main.cpp \
    player.cpp \
    playercontrols.cpp \
    playlistmodel.cpp
    
