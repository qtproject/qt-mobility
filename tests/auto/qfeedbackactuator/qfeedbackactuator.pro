TARGET = tst_qfeedbackactuator
CONFIG += testcase
INCLUDEPATH += ../../../src/feedback

include (../../../common.pri)

SOURCES += tst_qfeedbackactuator.cpp

symbian|linux-g++-maemo:DEFINES += HAVE_ACTUATORS

QT += core
CONFIG += mobility
MOBILITY = feedback

