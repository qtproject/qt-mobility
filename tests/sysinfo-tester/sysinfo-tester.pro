TARGET = sysinfo_tester
QT += gui network

INCLUDEPATH += ../../src/systeminfo
SOURCES  += main.cpp

include(../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo
CONFIG += console
