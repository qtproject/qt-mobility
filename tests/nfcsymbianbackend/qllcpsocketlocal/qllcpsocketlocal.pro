
QT       += testlib

TARGET = tst_qllcpsocketlocal
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += testcase
CONFIG   += mobility
MOBILITY = connectivity

TEMPLATE = app

INCLUDEPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common
SOURCES += tst_qllcpsocketlocal.cpp
symbian:TARGET.CAPABILITY = ALL -TCB
