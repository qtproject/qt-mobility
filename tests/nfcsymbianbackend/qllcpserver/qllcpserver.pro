QT       += testlib

TARGET = tst_qllcpserver
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += testcase
CONFIG   += mobility
MOBILITY = connectivity

TEMPLATE = app

INCLUDEPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common

HEADERS += ../common/qautomsgbox.h
SOURCES += tst_qllcpserver.cpp\
        ../common/qautomsgbox.cpp
symbian:TARGET.CAPABILITY = ALL -TCB


