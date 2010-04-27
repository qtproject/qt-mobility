TARGET = btchat

INCLUDEPATH += ../../src/connectivity
DEPENDPATH += ../../src/connectivity

include(../examples.pri)

CONFIG += mobility
MOBILITY = connectivity

SOURCES = \
    main.cpp \
    dialog.cpp

HEADERS = \
    dialog.h

FORMS = \
    dialog.ui

symbian {
    TARGET.CAPABILITY = LocalServices UserEnvironment ReadUserData WriteUserData NetworkServices
}
