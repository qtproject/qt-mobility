QT += testlib
TEMPLATE=app
TARGET=tst_qversitcontactimporter
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

HEADERS += ut_qversitcontactimporter.h
SOURCES += ut_qversitcontactimporter.cpp

qtAddLibrary(QtContacts_beta)
qtAddLibrary(QtVersit_beta)

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
