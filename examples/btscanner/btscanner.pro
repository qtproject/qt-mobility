TARGET = btscanner

INCLUDEPATH += ../../src/connectivity
DEPENDPATH += ../../src/connectivity

include(../examples.pri)

CONFIG += mobility
MOBILITY = connectivity

SOURCES = \
    main.cpp \
    device.cpp \
    service.cpp

HEADERS = \
    device.h \
    service.h

FORMS = \
    device.ui \
    service.ui

symbian {
    TARGET.CAPABILITY = LocalServices UserEnvironment ReadUserData WriteUserData NetworkServices
}

