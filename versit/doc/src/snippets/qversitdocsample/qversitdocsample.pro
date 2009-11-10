# #####################################################################
# Simple example of how to use the contacts API
# #####################################################################
TEMPLATE = app
TARGET =
include(../../../../common.pri)
DEPENDPATH += .
INCLUDEPATH += . \
    ../../../../../contacts/ \
    ../../../../../contacts/details/ \
    ../../../../../contacts/filters/ \
    ../../../../../contacts/requests/ \
    ../../../../../versit/

qtAddLibrary(QtVersit)
qtAddLibrary(QtContacts)

SOURCES += qversitdocsample.cpp
HEADERS += qversitdocsample.h
