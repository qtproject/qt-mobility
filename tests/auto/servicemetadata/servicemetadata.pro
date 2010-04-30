TARGET=tst_servicemetadata
INCLUDEPATH += ../../../src/serviceframework

wince*|symbian*: {
    addFiles.sources = testdata/*
    addFiles.path = testdata
    DEPLOYMENT += addFiles
}

wince* {
    DEFINES+= TESTDATA_DIR=\\\".\\\"
}else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}
               

CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_servicemetadata.cpp


symbian {
    TARGET.CAPABILITY = ALL -TCB
}

CONFIG += mobility
MOBILITY = serviceframework
