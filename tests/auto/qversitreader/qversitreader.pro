QT += testlib
TEMPLATE=app
TARGET=tst_qversitreader
CONFIG+=testcase

include(../../../common.pri)
DEFINES += QT_ASCII_CAST_WARNINGS

DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../ \
    ../../../src/versit \
    ../../../src/contacts \
    ../../../src/contacts/details \
    ../../../src/contacts/requests \
    ../../../src/contacts/filters

HEADERS += tst_qversitreader.h
SOURCES += tst_qversitreader.cpp

CONFIG += mobility
MOBILITY += contacts versit organizer

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
