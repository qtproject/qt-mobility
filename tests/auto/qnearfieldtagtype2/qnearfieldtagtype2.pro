SOURCES += tst_qnearfieldtagtype2.cpp
TARGET = tst_qnearfieldtagtype2
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/nfc
DEPENDPATH += ../../../src/connectivity/nfc

HEADERS += \
    qnearfieldtarget_emulator_p.h \
    qnearfieldmanager_emulator_p.h \
    targetemulator_p.h

SOURCES += \
    qnearfieldtarget_emulator.cpp \
    qnearfieldmanager_emulator.cpp \
    targetemulator.cpp


include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

DEFINES += SRCDIR=\\\"$$PWD\\\"

