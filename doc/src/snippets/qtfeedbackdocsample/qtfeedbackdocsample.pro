######################################################################
#
# Simple example of how to use the feedback API
#
######################################################################

TEMPLATE = lib
TARGET = qtfeedbackdocsample
include(../../../../common.pri)
INCLUDEPATH += ../../../../src/global \
               ../../../../src/feedback

DESTDIR = $$QT_MOBILITY_BUILD_TREE/lib
QMAKE_RPATHDIR+=$$OUTPUT_DIR/lib

CONFIG += mobility
MOBILITY = feedback

SOURCES += qtfeedbackdocsample.cpp

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    MMP_RULES+="EXPORTUNFROZEN"
}
