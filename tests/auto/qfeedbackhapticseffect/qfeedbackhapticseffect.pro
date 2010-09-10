TARGET = tst_qfeedbackhapticseffect
CONFIG += testcase
INCLUDEPATH += ../../../src/feedback

include (../../../common.pri)

SOURCES += tst_qfeedbackhapticseffect.cpp

QT += core
CONFIG += mobility
MOBILITY = feedback

