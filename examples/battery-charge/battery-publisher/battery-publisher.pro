TEMPLATE = app
TARGET = battery-publisher
INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe

HEADERS = batterypublisher.h

SOURCES = main.cpp \
          batterypublisher.cpp

FORMS = batterypublisher.ui

include(../../examples.pri)
CONFIG += mobility
MOBILITY = publishsubscribe
