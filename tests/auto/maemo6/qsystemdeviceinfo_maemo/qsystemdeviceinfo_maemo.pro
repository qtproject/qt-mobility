TARGET = tst_qsystemdeviceinfo_maemo
CONFIG += testcase
INCLUDEPATH += ../../../../src/systeminfo

include (../../../../common.pri)

SOURCES += tst_qsystemdeviceinfo_maemo.cpp

CONFIG += mobility
MOBILITY = systeminfo

QT += core dbus
