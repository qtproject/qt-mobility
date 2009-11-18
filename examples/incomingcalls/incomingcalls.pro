######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = incomingcalls
DEPENDPATH += .
INCLUDEPATH += . ../../src/contacts \
               ../../src/contacts/details \
               ../../src/contacts/requests \
               ../../src/contacts/filters


qtAddLibrary(QtContacts)

# Input
SOURCES += main.cpp \
           testmodelui.cpp \
           filterdialog.cpp \
           qcontactlistmodel.cpp

HEADERS += testmodelui.h \
           filterdialog.h \
           qcontactlistmodel.h \
           qcontactlistmodel_p.h

qtAddLibrary(QtContacts)
include(../examples.pri)
