SOURCES += tst_qnearfieldtagtype1.cpp
TARGET = tst_qnearfieldtagtype1
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/nfc
DEPENDPATH += ../../../src/connectivity/nfc

HEADERS += \
    qnearfieldmanagervirtualbase_p.h \
    qnearfieldmanager_emulator_p.h \
    qnearfieldtarget_emulator_p.h \
    targetemulator_p.h

SOURCES += \
    qnearfieldmanagervirtualbase.cpp \
    qnearfieldmanager_emulator.cpp \
    qnearfieldtarget_emulator.cpp \
    targetemulator.cpp


include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

DEFINES += SRCDIR=\\\"$$PWD/../nfcdata\\\"

