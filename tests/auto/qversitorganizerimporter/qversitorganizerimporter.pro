QT += testlib
TEMPLATE=app
TARGET=tst_qversitorganizerimporter
CONFIG+=testcase

include(../../../common.pri)
DEFINES += QT_ASCII_CAST_WARNINGS

DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../ \
    ../../../src/versit \
    ../../../src/organizer \
    ../../../src/organizer/details \
    ../../../src/organizer/requests \
    ../../../src/organizer/filters \
    ../../../src/organizer/items

HEADERS += tst_qversitorganizerimporter.h
SOURCES += tst_qversitorganizerimporter.cpp

CONFIG += mobility
MOBILITY += organizer versit contacts

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
