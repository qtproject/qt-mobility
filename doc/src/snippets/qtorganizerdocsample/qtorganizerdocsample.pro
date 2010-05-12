######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = lib
TARGET = qtorganizerdocsample
include(../../../../common.pri)
INCLUDEPATH += ../../../../src/global \
               ../../../../src/organizer

DESTDIR = $$QT_MOBILITY_BUILD_TREE/lib
QMAKE_RPATHDIR+=$$OUTPUT_DIR/lib

CONFIG += mobility console
MOBILITY = organizer

SOURCES += qtorganizerdocsample.cpp

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    MMP_RULES+="EXPORTUNFROZEN"
}
