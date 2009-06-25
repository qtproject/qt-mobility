TARGET=tst_databasemanager
QT = core sql
INCLUDEPATH += ../../../servicefw/serviceframework/servicemetadata \
               ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework/servicedatabase \
               ../../../servicefw/serviceframework

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
SOURCES += tst_databasemanagertest.cpp \
           ../qsfwtestutil.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

