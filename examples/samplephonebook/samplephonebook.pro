######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = samplephonebook
DEPENDPATH += .
INCLUDEPATH += . \
               ../../src/contacts\
               ../../src/contacts/filters \
               ../../src/contacts/requests \
               ../../src/contacts/details 

qtAddLibrary(QtContacts)

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
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
                        SwEvent
}
include(../examples.pri)
qtAddLibrary(QtContacts)
