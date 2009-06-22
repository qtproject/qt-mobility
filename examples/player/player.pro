TEMPLATE = app

INCLUDEPATH += $$PWD/../../plugins/wmp

LIBS += -L$$OUT_PWD/../../lib -lqwmp

DEPENDPATH += $$PWD/../../plugins/wmp

SOURCES = \
    main.cpp \
    player.cpp
    
HEADERS = \
    player.h
    