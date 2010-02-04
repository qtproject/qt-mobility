######################################################################
#
# Simple example of how to use the versit API
#
######################################################################

TEMPLATE = app
TARGET = qtversitdocsample
include(../../../../common.pri)
INCLUDEPATH += ../../../../src/global \
               ../../../../src/contacts \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters \
               ../../../../src/contacts/details \
               ../../../../src/versit

DESTDIR = $$QT_MOBILITY_BUILD_TREE/bin/examples
QMAKE_RPATHDIR+=$$OUTPUT_DIR/lib

CONFIG += mobility console
MOBILITY = contacts versit

SOURCES += qtversitdocsample.cpp
