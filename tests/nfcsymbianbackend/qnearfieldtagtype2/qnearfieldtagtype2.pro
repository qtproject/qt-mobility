#-------------------------------------------------
#
# Project created by QtCreator 2010-11-12T15:06:08
#
#-------------------------------------------------

QT       += testlib

TARGET = tst_qnearfieldtagtype2
CONFIG   += console
CONFIG   -= app_bundle

CONFIG   += mobility
CONFIG   += testcase
MOBILITY = connectivity

TEMPLATE = app

INCLUDEPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common
HEADERS += ../common/qnfctagtestcommon.h
SOURCES += tst_qnearfieldtagtype2.cpp \
           ../common/qnfctagtestcommon.cpp
