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
    LIBS += -lmsgs -limcm -lsmcm -lgsmu -letext -lbafl
    INCLUDEPATH += /epoc32/include/app
    INCLUDEPATH += /epoc32/include/platform/app
    INCLUDEPATH += /epoc32/include/applications
}

maemo {
    QT += dbus xml gui
}
