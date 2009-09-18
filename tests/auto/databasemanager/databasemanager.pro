TARGET=tst_databasemanager
QT = core sql
INCLUDEPATH += ../../../serviceframework

wince*|symbian*: {
    addFiles.sources = testdata/*
    addFiles.path = testdata
    DEPLOYMENT += addFiles
}

wince* {
    DEFINES+= SRCDIR=\\\".\\\"
} !symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

CONFIG+=testcase

include(../../../common.pri)

HEADERS += ../qsfwtestutil.h
SOURCES += tst_databasemanager.cpp \
           ../qsfwtestutil.cpp

qtAddLibrary(QtServiceFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

