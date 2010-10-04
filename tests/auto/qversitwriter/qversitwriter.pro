QT += testlib
TEMPLATE=app
TARGET=tst_qversitwriter
CONFIG+=testcase

include(../../../common.pri)

DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../ \
    ../../../src/versit

HEADERS += tst_qversitwriter.h
SOURCES += tst_qversitwriter.cpp

CONFIG += mobility
MOBILITY += versit

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
