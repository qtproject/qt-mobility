TEMPLATE = app
TARGET = tst_bm_messaging
CONFIG += testcase

SOURCES += tst_messaging.cpp

QT += core \
      network


INCLUDEPATH += ../../../src/messaging

include(../../../common.pri)

CONFIG += mobility

MOBILITY = messaging


symbian {
    TARGET.CAPABILITY = All -TCB
    LIBS += msgs.lib msgs_autoshutdown.lib imcm.lib smcm.lib gsmu.lib etext.lib bafl.lib
}

maemo {
    QT += dbus xml gui
}
