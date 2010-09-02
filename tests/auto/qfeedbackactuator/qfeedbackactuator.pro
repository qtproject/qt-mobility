TARGET = tst_qfeedbackactuator
CONFIG += testcase
INCLUDEPATH += ../../../src/feedback

include (../../../common.pri)

SOURCES += tst_qfeedbackactuator.cpp

symbian|maemo5|maemo6:DEFINES += HAVE_ACTUATORS

QT += core
CONFIG += mobility
MOBILITY = feedback

