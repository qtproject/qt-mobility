######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = qtorganizerdocsample
include(../../../../common.pri)
INCLUDEPATH += ../../../../src/global \
               ../../../../src/organizer \
               ../../../../src/organizer/details \
               ../../../../src/organizer/requests \
               ../../../../src/organizer/items \
               ../../../../src/organizer/filters

DESTDIR = $$QT_MOBILITY_BUILD_TREE/bin
QMAKE_RPATHDIR+=$$OUTPUT_DIR/lib

CONFIG += mobility console
MOBILITY = organizer

SOURCES += qtorganizerdocsample.cpp

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    MMP_RULES+="EXPORTUNFROZEN"
}
