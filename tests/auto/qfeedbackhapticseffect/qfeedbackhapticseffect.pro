TARGET = tst_qfeedbackhapticseffect
CONFIG += testcase
INCLUDEPATH += ../../../src/feedback

include (../../../common.pri)

SOURCES += tst_qfeedbackhapticseffect.cpp

symbian|maemo6:DEFINES += SUPPORTS_THEME

QT += core
CONFIG += mobility
MOBILITY = feedback

