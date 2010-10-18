QT += testlib
TEMPLATE=app
TARGET=tst_qversitdocument
CONFIG+=testcase
CONFIG+=parallel_test

include(../../../common.pri)

DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../ \
    ../../../src/versit

HEADERS += tst_qversitdocument.h
SOURCES += tst_qversitdocument.cpp

CONFIG += mobility
MOBILITY += versit

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
