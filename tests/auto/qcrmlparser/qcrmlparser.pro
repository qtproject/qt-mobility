TARGET = tst_qcrmlparser
INCLUDEPATH += ../../../src/publishsubscribe
CONFIG += testcase
QT = core xml
include(../../../common.pri)

# Input
SOURCES += ../../../src/publishsubscribe/psmapperserver_symbian/qcrmlparser.cpp \ 
        tst_qcrmlparser.cpp

wince* {
    DEFINES+= TESTDATA_DIR=\\\".\\\"
}else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

wince*|symbian*: {
    addFiles.sources = testdata/*
    addFiles.path = testdata
    DEPLOYMENT += addFiles
}

symbian:TARGET.CAPABILITY = ALL -TCB
