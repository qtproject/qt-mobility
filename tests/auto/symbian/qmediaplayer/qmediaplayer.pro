TARGET = tst_qmediaplayer
INCLUDEPATH += ../../../../include
CONFIG += testcase

QT += testlib

SOURCES += tst_qmediaplayer.cpp

include (../../../../common.pri)

qtAddLibrary(QtMedia)

