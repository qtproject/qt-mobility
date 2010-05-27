TEMPLATE = app
TARGET = tst_smsservice

CONFIG += testcase
QT += testlib

include(../../../common.pri)
include(../support/support.pri)

qtAddLibrary(QtMessaging)
INCLUDEPATH += ../../../src/messaging

maemo6 {
} else {
DEFINES += Q_WS_MAEMO_6
}

SOURCES += \
    tst_smsservice.cpp
