######################################################################
#
# Simple example of how to use the feedback API
#
######################################################################

TEMPLATE = lib
TARGET = qtfeedbackdocsample
include(../../../../features/basic_examples_setup.pri)
INCLUDEPATH += ../../../../src/global \
               ../../../../src/feedback

CONFIG += mobility
MOBILITY = feedback

SOURCES += qtfeedbackdocsample.cpp

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    MMP_RULES+="EXPORTUNFROZEN"
}
