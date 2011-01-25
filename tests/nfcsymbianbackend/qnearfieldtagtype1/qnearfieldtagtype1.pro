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
           ../common/qdummyslot.h \
           ../common/qautomsgbox.h

SOURCES += tst_qnearfieldtagtype1.cpp \
           ../common/qnfctagtestcommon.cpp \
           ../common/qdummyslot.cpp \
           ../common/qautomsgbox.cpp

symbian:TARGET.CAPABILITY = ALL -TCB
