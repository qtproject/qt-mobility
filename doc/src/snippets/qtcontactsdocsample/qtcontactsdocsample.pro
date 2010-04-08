######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = qtcontactsdocsample
include(../../../../common.pri)
INCLUDEPATH += ../../../../src/global \
               ../../../../src/contacts \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters \
               ../../../../src/contacts/details

DESTDIR = $$QT_MOBILITY_BUILD_TREE/bin/examples
QMAKE_RPATHDIR+=$$OUTPUT_DIR/lib

CONFIG += mobility console
MOBILITY = contacts

SOURCES += qtcontactsdocsample.cpp qtcontactsdocsampleasync.cpp
HEADERS += requestexample.h
