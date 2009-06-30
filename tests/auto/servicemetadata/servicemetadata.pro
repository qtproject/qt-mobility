TARGET=tst_servicemetadata
INCLUDEPATH += ../../../servicefw/serviceframework/servicemetadata \
               ../../../servicefw/serviceframework/serviceresolution \
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

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_servicemetadata.cpp


symbian {
    TARGET.CAPABILITY = ALL -TCB
}
