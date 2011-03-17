TEMPLATE = app
TARGET = contacts-tester
DEPENDPATH += .
INCLUDEPATH += .

include(../../common.pri)
#not really a test case but deployment happens same way
CONFIG += testcase

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
