TEMPLATE = app
CONFIG += example

include (../../common.pri)

INCLUDEPATH += $$PWD/../../multimedia

LIBS += -lQtMedia

HEADERS = \
    player.h \
    playercontrols.h \
    playlistmodel.h
  
SOURCES = \
    main.cpp \
    player.cpp \
    playercontrols.cpp \
    playlistmodel.cpp
    
