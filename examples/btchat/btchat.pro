TARGET = btchat

INCLUDEPATH += ../../src/connectivity ../../src/connectivity/bluetooth
DEPENDPATH += ../../src/connectivity ../../src/connectivity/bluetooth

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
