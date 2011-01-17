TARGET = bttennis

INCLUDEPATH += ../../src/connectivity/bluetooth
DEPENDPATH += ../../src/connectivity/bluetooth

include(../examples.pri)

CONFIG += mobility
MOBILITY = connectivity

SOURCES = \
    main.cpp \
    board.cpp \
    tennis.cpp \
    controller.cpp \
    tennisserver.cpp \
    tennisclient.cpp \
    tennisview.cpp

HEADERS = \
    board.h \
    tennis.h \
    controller.h \
    tennisserver.h \
    tennisclient.h \
    tennisview.h

FORMS = \
    tennis.ui

symbian {
    TARGET.CAPABILITY = LocalServices UserEnvironment ReadUserData WriteUserData NetworkServices
}
