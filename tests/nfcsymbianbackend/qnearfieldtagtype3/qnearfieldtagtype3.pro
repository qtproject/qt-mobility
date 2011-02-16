QT       += testlib

TARGET = tst_qnearfieldtagtype3
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += mobility
CONFIG   += testcase
MOBILITY = connectivity

TEMPLATE = app

INCLUDEPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common
HEADERS += ../common/qnfctagtestcommon.h \
           ../common/qdummyslot.h

SOURCES += tst_qnearfieldtagtype3.cpp \
           ../common/qdummyslot.cpp

symbian:TARGET.CAPABILITY = ALL -TCB

