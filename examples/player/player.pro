TEMPLATE = app

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints
include (../../common.pri)

LIBS += -lQtMedia

QMAKE_RPATHDIR += $$DESTDIR


HEADERS = \
    player.h \
    playercontrols.h \
    playlistmodel.h
  
SOURCES = \
    main.cpp \
    player.cpp \
    playercontrols.cpp \
    playlistmodel.cpp
    
