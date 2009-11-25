QT += testlib
TEMPLATE=app
TARGET=tst_qversitreader
CONFIG+=testcase

include(../../../common.pri)
DEFINES += BUILD_QTVERSIT QT_ASCII_CAST_WARNINGS

DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../ \
    ../../../src/versit \
    ../../../src/contacts \
    ../../../src/contacts/details \
    ../../../src/contacts/requests \
    ../../../src/contacts/filters

HEADERS += ut_qversitreader.h
SOURCES += ut_qversitreader.cpp

qtAddLibrary(QtContacts_beta)
qtAddLibrary(QtVersit_beta)

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
