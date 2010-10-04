QT += testlib
TEMPLATE=app
TARGET=tst_qversitproperty
CONFIG+=testcase
CONFIG+=parallel_test

include(../../../common.pri)

DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../ \
    ../../../src/versit

HEADERS += tst_qversitproperty.h
SOURCES += tst_qversitproperty.cpp

CONFIG += mobility
MOBILITY += versit

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
