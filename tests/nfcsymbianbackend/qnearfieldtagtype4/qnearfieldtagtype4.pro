#-------------------------------------------------
#
# Project created by QtCreator 2010-11-12T15:07:35
#
#-------------------------------------------------

QT       += testlib

TARGET = tst_qnearfieldtagtype4
CONFIG   += console
CONFIG   -= app_bundle

CONFIG   += mobility
CONFIG   += testcase
MOBILITY = connectivity

TEMPLATE = app


INCLUDEPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common
HEADERS += ../common/qnfctagtestcommon.h
SOURCES += tst_qnearfieldtagtype4.cpp \
           ../common/qnfctagtestcommon.cpp
symbian:TARGET.CAPABILITY = ALL -TCB
