TEMPLATE = app

INCLUDEPATH += $$PWD/../../multimedia

LIBS += -L$$OUT_PWD/../../lib -lQtMedia

DESTDIR = ../../bin

HEADERS = \
    player.h \
    playlistmodel.h
  
SOURCES = \
    main.cpp \
    player.cpp \
    playlistmodel.cpp
    
