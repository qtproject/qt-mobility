TEMPLATE = app
TARGET = contacts-tester
DEPENDPATH += .
INCLUDEPATH += .

include(../../common.pri)

CONFIG += mobility
MOBILITY += contacts

INCLUDEPATH += . \
               ../../src/contacts\
               ../../src/contacts/filters \
               ../../src/contacts/requests \
               ../../src/contacts/details \
               ../../src/global

# Input
HEADERS += mainwindow.h
SOURCES += main.cpp mainwindow.cpp
