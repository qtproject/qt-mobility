#-------------------------------------------------
#
# Project created by QtCreator 2010-11-12T15:06:51
#
#-------------------------------------------------

QT       += testlib

TARGET = tst_qnearfieldtagtype3
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


CONFIG   += mobility
CONFIG   += testcase
MOBILITY = connectivity

TEMPLATE = app

INCLUDEPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common
HEADERS += ../common/qnfctagtestcommon.h
SOURCES += tst_qnearfieldtagtype3.cpp \
           ../common/qnfctagtestcommon.cpp
