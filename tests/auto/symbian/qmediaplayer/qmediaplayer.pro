TARGET = tst_qmediaplayer
INCLUDEPATH += ../../../../src/multimedia \
               ../../../../include 
CONFIG += testcase

QT += testlib

SOURCES += tst_qmediaplayer.cpp

include (../../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia
