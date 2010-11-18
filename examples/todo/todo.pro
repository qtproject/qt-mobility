
TEMPLATE = app
TARGET = todo
include(../examples.pri)
DEPENDPATH += .
CONFIG += mobility
MOBILITY = organizer

HEADERS += todoeditor.h \
           window.h

SOURCES += window.cpp \
           todoeditor.cpp \
           main.cpp

INCLUDEPATH += . \
    ../../include \
    ../../src/organizer/ \
    ../../src/organizer/items \
    ../../src/organizer/details \
    ../../src/organizer/filters \
    ../../src/organizer/requests \
    ../../src/global
