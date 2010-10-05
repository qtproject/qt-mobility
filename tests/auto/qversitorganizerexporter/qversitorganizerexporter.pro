QT += testlib
TEMPLATE=app
TARGET=tst_qversitorganizerexporter
CONFIG+=testcase
CONFIG+=parallel_test

include(../../../common.pri)

DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../ \
    ../../../src/versit \
    ../../../src/versitorganizer \
    ../../../src/organizer \
    ../../../src/organizer/details \
    ../../../src/organizer/requests \
    ../../../src/organizer/filters \
    ../../../src/organizer/items

HEADERS += tst_qversitorganizerexporter.h
SOURCES += tst_qversitorganizerexporter.cpp

CONFIG += mobility
MOBILITY += organizer versit

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
