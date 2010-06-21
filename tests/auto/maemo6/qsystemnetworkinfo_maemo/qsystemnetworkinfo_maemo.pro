TARGET = tst_qsystemnetworkinfo_maemo
CONFIG += testcase
INCLUDEPATH += ../../../../src/systeminfo

include (../../../../common.pri)

SOURCES += tst_qsystemnetworkinfo_maemo.cpp

CONFIG += mobility
MOBILITY = systeminfo

QT += core dbus
