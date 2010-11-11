SOURCES += tst_qnearfieldtagtype1.cpp
TARGET = tst_qnearfieldtagtype1
CONFIG += testcase

QT = core

INCLUDEPATH += ../../../src/connectivity/nfc
DEPENDPATH += ../../../src/connectivity/nfc

HEADERS += \
    qnearfieldmanager_emulator_p.h \
    targetemulator_p.h

SOURCES += \
    qnearfieldmanager_emulator.cpp \
    targetemulator.cpp


include(../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

DEFINES += SRCDIR=\\\"$$PWD\\\"

