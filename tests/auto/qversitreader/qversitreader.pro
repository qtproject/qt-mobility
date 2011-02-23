QT += testlib
TEMPLATE=app
TARGET=tst_qversitreader
CONFIG+=testcase

include(../../../common.pri)

requires(contains(build_unit_tests, yes))

DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../ \
    ../../../src/versit

HEADERS += tst_qversitreader.h
SOURCES += tst_qversitreader.cpp

CONFIG += mobility
MOBILITY += versit

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
