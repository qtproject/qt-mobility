QT += testlib
TEMPLATE=app
TARGET=tst_qversitcontactexporter
CONFIG+=testcase
CONFIG+=parallel_test

include(../../../common.pri)

DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../ \
    ../../../src/versit \
    ../../../src/contacts \
    ../../../src/contacts/details \
    ../../../src/contacts/requests \
    ../../../src/contacts/filters

HEADERS += tst_qversitcontactexporter.h
SOURCES += tst_qversitcontactexporter.cpp

CONFIG += mobility
MOBILITY += contacts versit

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
