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
HEADERS += ../common/qautomsgbox.h
SOURCES += tst_qllcpsockettype2.cpp \
        ../common/qautomsgbox.cpp
symbian:TARGET.CAPABILITY = ALL -TCB
