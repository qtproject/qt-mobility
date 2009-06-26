TEMPLATE = app

INCLUDEPATH += $$PWD/../../plugins/wmp

LIBS += -L$$OUT_PWD/../../lib -lqwmp

DEPENDPATH += $$PWD/../../plugins/wmp

HEADERS = \
    player.h \
    playlistmodel.h
  
SOURCES = \
    main.cpp \
    player.cpp \
    playlistmodel.cpp
    