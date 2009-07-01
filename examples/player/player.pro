TEMPLATE = app

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints

LIBS += -L$$OUT_PWD/../../lib -lQtMedia

DESTDIR = ../../bin

HEADERS = \
    player.h \
    playlistmodel.h
  
SOURCES = \
    main.cpp \
    player.cpp \
    playlistmodel.cpp
    
