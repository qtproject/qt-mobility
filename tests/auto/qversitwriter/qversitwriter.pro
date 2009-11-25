QT += testlib
TEMPLATE=app
TARGET=tst_qcontact
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

HEADERS +=    ut_qversitwriter.h

SOURCES +=     ../../../src/versit/qversitwriter.cpp \
    ut_qversitwriter.cpp

qtAddLibrary(QtContacts)

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
