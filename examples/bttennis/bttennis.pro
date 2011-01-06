TARGET = bttennis

INCLUDEPATH += ../../src/connectivity/bluetooth
DEPENDPATH += ../../src/connectivity/bluetooth

include(../examples.pri)

CONFIG += mobility
MOBILITY = connectivity

SOURCES = \
    main.cpp \
    remoteselector.cpp \
    board.cpp \
    tennis.cpp \
    controller.cpp \
    tennisserver.cpp \
    tennisclient.cpp

HEADERS = \
    remoteselector.h \
    board.h \
    tennis.h \
    controller.h \
    tennisserver.h \
    tennisclient.h

FORMS = \
    tennis.ui \
    remoteselector.ui

symbian {
    TARGET.CAPABILITY = LocalServices UserEnvironment ReadUserData WriteUserData NetworkServices
}
