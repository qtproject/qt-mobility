######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = samplephonebook
include(../examples.pri)
DEPENDPATH += .
INCLUDEPATH += . \
               ../../src/contacts\
               ../../src/contacts/filters \
               ../../src/contacts/requests \
               ../../src/contacts/details \
               ../../src/versit

CONFIG += mobility
MOBILITY = contacts
contains(mobility_modules,versit) {
    MOBILITY += versit organizer
    DEFINES += BUILD_VERSIT
}

# Input
SOURCES += main.cpp \
           phonebook.cpp \
           contacteditor.cpp \
           contactlistpage.cpp \
           filterpage.cpp
HEADERS += phonebook.h \
           contacteditor.h \
           contactlistpage.h \
           filterpage.h

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
}
