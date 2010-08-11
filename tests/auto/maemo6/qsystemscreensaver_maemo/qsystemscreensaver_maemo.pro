TARGET = tst_qsystemscreensaver_maemo
CONFIG += testcase
INCLUDEPATH += ../../../../src/systeminfo

include (../../../../common.pri)

SOURCES += tst_qsystemscreensaver_maemo.cpp

CONFIG += mobility
MOBILITY = systeminfo

QT += core
