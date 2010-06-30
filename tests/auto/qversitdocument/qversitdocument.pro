QT += testlib
TEMPLATE=app
TARGET=tst_qversitdocument
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

HEADERS += tst_qversitdocument.h
SOURCES += tst_qversitdocument.cpp

CONFIG += mobility
MOBILITY += contacts versit organizer

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
