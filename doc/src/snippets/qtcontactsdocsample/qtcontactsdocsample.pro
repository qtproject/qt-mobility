######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = qtcontactsdocsample
include(../../../../common.pri)
DEPENDPATH += .
INCLUDEPATH += . $$SOURCE_DIR/contacts $$SOURCE_DIR/contacts/details $$SOURCE_DIR/contacts/filters $$SOURCE_DIR/contacts/requests
LIBS += -lQtContacts

SOURCES += qtcontactsdocsample.cpp
