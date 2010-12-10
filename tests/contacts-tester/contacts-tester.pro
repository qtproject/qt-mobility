TEMPLATE = app
TARGET = contacts-tester
DEPENDPATH += .
INCLUDEPATH += .

CONFIG += mobility
MOBILITY += contacts

INCLUDEPATH += . \
               ../../src/contacts\
               ../../src/contacts/filters \
               ../../src/contacts/requests \
               ../../src/contacts/details

# Input
HEADERS += mainwindow.h
SOURCES += main.cpp mainwindow.cpp
