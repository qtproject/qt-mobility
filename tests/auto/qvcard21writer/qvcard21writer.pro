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

HEADERS +=    ut_qvcard21writer.h

SOURCES +=     ../../../src/versit/qvcard21writer.cpp \
    ut_qvcard21writer.cpp

qtAddLibrary(QtContacts)

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
