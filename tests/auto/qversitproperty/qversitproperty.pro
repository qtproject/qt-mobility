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

HEADERS +=    ut_qversitproperty.h

SOURCES +=     ../../../src/versit/qversitproperty.cpp \
    ut_qversitproperty.cpp

qtAddLibrary(QtContacts)

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
