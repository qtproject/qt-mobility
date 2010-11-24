QT       += testlib

TARGET = tst_qllcpsockettype2
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += testcase
CONFIG   += mobility
MOBILITY = connectivity

TEMPLATE = app

INCLUDEPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common
SOURCES += tst_qllcpsockettype2.cpp
symbian:TARGET.CAPABILITY = ALL - TCB