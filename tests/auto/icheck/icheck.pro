REL_PATH_TO_SRC = ../../../tools/icheck/ichecklib/src

QT += testlib xml
TARGET=tst_icheck
CONFIG+=testcase
QT -= gui
DEFINES += ICHECK_BUILD ICHECKTESTBUILD

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
    $$REL_PATH_TO_SRC/tools/ICheck \
    $$REL_PATH_TO_SRC/libs/cplusplus

include(../../../tools/icheck/ichecklib/ichecklib.pri)

CONFIG += mobility
MOBILITY = icheck

SOURCES  += tst_icheck.cpp

