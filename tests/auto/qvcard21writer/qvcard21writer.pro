QT += testlib
TEMPLATE=app
TARGET=tst_qvcard21writer
CONFIG+=testcase

include(../../../common.pri)

DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../ \
    ../../../src/versit

HEADERS += tst_qvcard21writer.h
SOURCES += tst_qvcard21writer.cpp

CONFIG += mobility
MOBILITY += versit

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
