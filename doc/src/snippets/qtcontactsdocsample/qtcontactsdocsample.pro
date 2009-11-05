######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = qtcontactsdocsample
include(../../../../common.pri)
INCLUDEPATH += ../../../../contacts \
                ../../../../contacts/requests \
                ../../../../contacts/filters \
                ../../../../contacts/details

DESTDIR = $$QT_MOBILITY_BUILD_TREE/bin/examples
QMAKE_RPATHDIR+=$$OUTPUT_DIR/lib
qtAddLibrary(QtContacts)

SOURCES += qtcontactsdocsample.cpp
HEADERS += requestexample.h
