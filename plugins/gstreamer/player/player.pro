TEMPLATE = app

INCLUDEPATH += $$PWD/.. $$PWD/../../../multimedia

LIBS += -L$$OUT_PWD/../../../lib -lQtMedia -lgstengine

INCLUDEPATH += . ../../multimedia /usr/include/gstreamer-0.10 /usr/include/glib-2.0 /usr/lib/glib-2.0/include/

QMAKE_CXXFLAGS += $$QT_CFLAGS_GSTREAMER
LIBS += $$QT_LIBS_GSTREAMER -lgstinterfaces-0.10 -lgstvideo-0.10 -lgstbase-0.10 -lgstaudio-0.10 -lQtMedia


HEADERS = \
    player.h

  
SOURCES = \
    main.cpp \
    player.cpp

    
