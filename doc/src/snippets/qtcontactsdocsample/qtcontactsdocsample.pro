######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = qtcontactsdocsample
DEPENDPATH += .
INCLUDEPATH += . ../../../../contacts
LIBS += -lQtContacts

include(../../../../common.pri)
SOURCES += qtcontactsdocsample.cpp
