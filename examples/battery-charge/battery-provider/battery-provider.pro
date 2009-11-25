TEMPLATE = app
TARGET = battery-provider
INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe

HEADERS = batteryprovider.h

SOURCES = main.cpp \
          batteryprovider.cpp

FORMS = batteryprovider.ui

include(../../examples.pri)
CONFIG += mobility
MOBILITY = publishsubscribe
