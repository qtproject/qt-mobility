TEMPLATE = app
TARGET = simple

INCLUDEPATH += ../../context
DEPENDPATH += ../../context

SOURCES = main.cpp

include(../../common.pri)
LIBS += -lQtContextFramework
