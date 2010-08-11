######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = qtorganizerdocsample
include(../../../../features/basic_examples_setup.pri)
INCLUDEPATH += ../../../../src/global \
               ../../../../src/organizer \
               ../../../../src/organizer/details \
               ../../../../src/organizer/requests \
               ../../../../src/organizer/items \
               ../../../../src/organizer/filters

CONFIG += mobility console
MOBILITY = organizer

SOURCES += qtorganizerdocsample.cpp

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    MMP_RULES+="EXPORTUNFROZEN"
}
