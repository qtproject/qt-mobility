QT += testlib
TEMPLATE = app
TARGET = tst_qversit

wince* {
    DEFINES+= TESTDATA_DIR=\\\".\\\"
}else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

CONFIG += testcase
include(../../../common.pri)
DEFINES += QT_ASCII_CAST_WARNINGS
DEPENDPATH += .
INCLUDEPATH += . \
    ../../ \
    ../../../src/versit \
    ../../../src/contacts \
    ../../../src/contacts/details \
    ../../../src/contacts/requests \
    ../../../src/contacts/filters
HEADERS += tst_qversit.h
SOURCES += tst_qversit.cpp
CONFIG += mobility
MOBILITY = contacts \
    versit
symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
