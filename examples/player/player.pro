TEMPLATE = app
CONFIG += example

include (../../common.pri)

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints

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
    
