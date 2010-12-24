TARGET = tst_qfeedbacknoplugins
CONFIG += testcase
INCLUDEPATH += ../../../src/feedback

include (../../../common.pri)

SOURCES += tst_qfeedbacknoplugins.cpp

QT += core
CONFIG += mobility
MOBILITY = feedback

