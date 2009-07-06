######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = simplecontacts
DEPENDPATH += .
INCLUDEPATH += . ../../contacts ../../contacts/$${CONTACTS_BACKEND} ../../util
LIBS += -lQtContacts

include(../../common.pri)

# Input
SOURCES += simplecontacts.cpp
