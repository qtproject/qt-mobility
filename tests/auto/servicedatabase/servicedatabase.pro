TARGET=tst_servicedatabase
QT = core sql
INCLUDEPATH += ../../../serviceframework
DEPENDPATH += ../../../serviceframework

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

# Input 
SOURCES += tst_servicedatabase.cpp

LIBS += -lQtServiceFramework

symbian {
    SOURCES += servicedatabase.cpp
    HEADERS += servicedatabase_p.h
    TARGET.CAPABILITY = ALL -TCB
}

