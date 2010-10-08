QT += testlib
TEMPLATE = app
TARGET = tst_qversit

symbian*: {
    VERSIT_TESTDATA.sources = testdata/*
    VERSIT_TESTDATA.path = testdata
    DEPLOYMENT += VERSIT_TESTDATA
}

wince* {
    DEFINES+= TESTDATA_DIR=\\\"./\\\"
}else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

CONFIG += testcase
include(../../../common.pri)
DEPENDPATH += .
INCLUDEPATH += . \
    ../../ \
    ../../../src/versit \
    ../../../src/versitorganizer \
    ../../../src/contacts \
    ../../../src/contacts/details \
    ../../../src/contacts/requests \
    ../../../src/contacts/filters \
    ../../../src/organizer \
    ../../../src/organizer/details \
    ../../../src/organizer/requests \
    ../../../src/organizer/filters \
    ../../../src/organizer/items
HEADERS += tst_qversit.h
SOURCES += tst_qversit.cpp
CONFIG += mobility
MOBILITY += contacts versit organizer
symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
