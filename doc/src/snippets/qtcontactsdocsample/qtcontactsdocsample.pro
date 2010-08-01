######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = lib
TARGET = qtcontactsdocsample
include(../../../../features/basic_examples_setup.pri)
INCLUDEPATH += ../../../../src/global \
               ../../../../src/contacts \
               ../../../../src/contacts/requests \
               ../../../../src/contacts/filters \
               ../../../../src/contacts/details

CONFIG += mobility console
MOBILITY = contacts

SOURCES += qtcontactsdocsample.cpp qtcontactsdocsampleasync.cpp
HEADERS += requestexample.h

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    MMP_RULES+="EXPORTUNFROZEN"
}
