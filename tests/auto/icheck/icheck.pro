REL_PATH_TO_SRC = ../../../tools/icheck/parser/src

QT += testlib xml
TARGET=tst_icheck
CONFIG+=testcase
QT -= gui
DEFINES += ICHECK_BUILD ICHECK_APP_BUILD

wince*|symbian*: {
    addFiles.sources = testdata/*
    addFiles.path = testdata
    DEPLOYMENT += addFiles
}

wince* {
    DEFINES += TEADATA_DIR=\\\".\\\"
} !symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}


include(../../../common.pri)

INCLUDEPATH += . \
    $$REL_PATH_TO_SRC/../ \
    $$REL_PATH_TO_SRC/global \
    $$REL_PATH_TO_SRC/plugins \
    $$REL_PATH_TO_SRC/libs \
    $$REL_PATH_TO_SRC/shared/cplusplus \
    $$REL_PATH_TO_SRC/libs/cplusplus \
    ../../../tools/icheck

include(../../../tools/icheck/parser/ichecklib.pri)

SOURCES += tst_icheck.cpp \
    ../../../tools/icheck/parsemanager.cpp \
    ../../../tools/icheck/ichecklib.cpp

HEADERS += ../../../tools/icheck/parsemanager.h \
    ../../../tools/icheck/ichecklib.h

