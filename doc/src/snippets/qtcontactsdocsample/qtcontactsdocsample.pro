######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = qtcontactsdocsample
include(../../../../common.pri)
DEPENDPATH += .
INCLUDEPATH += .

qtAddLibrary(QtContacts)

SOURCES += qtcontactsdocsample.cpp
HEADERS += requestexample.h
