TARGET = tst_qfeedbackplugin
CONFIG += testcase
INCLUDEPATH += ../../../../src/feedback

include (../../../../common.pri)

SOURCES += tst_qfeedbackplugin.cpp

symbian|linux-g++-maemo:DEFINES += HAVE_ACTUATORS

QT += core
CONFIG += mobility
MOBILITY = feedback

