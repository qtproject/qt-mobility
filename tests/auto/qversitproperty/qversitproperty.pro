QT += testlib
TEMPLATE=app
TARGET=tst_qversitproperty
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

HEADERS += ut_qversitproperty.h
SOURCES += ut_qversitproperty.cpp

CONFIG += mobility
MOBILITY = contacts versit

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
