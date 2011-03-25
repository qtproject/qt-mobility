QT       += testlib

TARGET = tst_qnearfieldmanager
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += mobility
CONFIG   += testcase
MOBILITY = connectivity

TEMPLATE = app

INCLUDEPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common

HEADERS += ../common/qautomsgbox.h
SOURCES += tst_qnearfieldmanager.cpp \
        ../common/qautomsgbox.cpp
symbian:TARGET.CAPABILITY = ALL -TCB
