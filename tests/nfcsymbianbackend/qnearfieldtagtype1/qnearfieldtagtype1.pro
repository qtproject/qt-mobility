QT       += testlib

TARGET = tst_qnearfieldtagtype1
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += mobility
CONFIG   += testcase
MOBILITY = connectivity

TEMPLATE = app


INCLUDEPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common
HEADERS += ../common/qnfctagtestcommon.h \
           qdummyslot.h

SOURCES += tst_qnearfieldtagtype1.cpp \
           qdummyslot.cpp \
           ../common/qnfctagtestcommon.cpp
symbian:TARGET.CAPABILITY = ALL -TCB
