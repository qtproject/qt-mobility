TEMPLATE = app
TARGET = tst_smsstore

CONFIG += testcase
QT += testlib

include(../../../common.pri)
include(../support/support.pri)

CONFIG += mobility
MOBILITY = messaging
INCLUDEPATH += ../../../src/messaging

maemo6 {
} else {
DEFINES += Q_WS_MAEMO_6
}

SOURCES += \
    tst_smsstore.cpp

