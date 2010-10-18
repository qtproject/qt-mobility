QT += testlib
TEMPLATE = app
TARGET = tst_bm_versit

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
    ../../../src/contacts \
    ../../../src/contacts/details \
    ../../../src/contacts/requests \
    ../../../src/contacts/filters

SOURCES += tst_versit.cpp
CONFIG += mobility
MOBILITY += contacts versit
symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
