TEMPLATE = app
TARGET = tst_bm_bearer
CONFIG += testcase

SOURCES += tst_bm_bearer.cpp

QT += core \
      network


INCLUDEPATH += ../../../src/bearer

include(../../../common.pri)

CONFIG += mobility

MOBILITY = bearer


symbian {
    TARGET.CAPABILITY = All -TCB  
    INCLUDEPATH += $$(EPOCROOT)epoc32/include/app
    INCLUDEPATH += $$(EPOCROOT)epoc32/include/platform/app
}

maemo5|maemo6 {
    CONFIG += link_pkgconfig

    PKGCONFIG += conninet
}
