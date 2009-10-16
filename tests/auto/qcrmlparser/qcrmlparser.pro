TARGET = tst_qcrmlparser
INCLUDEPATH += ../../../context
DEPENDPATH += ../../../context
CONFIG += testcase
QT = core xml
include(../../../common.pri)

# Input
SOURCES += ../../../context/qcrmlparser.cpp \ 
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
