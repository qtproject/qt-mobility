
QT       += testlib

TARGET = tst_qllcpsocketremote
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += testcase
CONFIG   += mobility
MOBILITY = connectivity

TEMPLATE = app

INCLUDEPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common
SOURCES += tst_qllcpsocketremote.cpp
symbian:TARGET.CAPABILITY = ALL -TCB
