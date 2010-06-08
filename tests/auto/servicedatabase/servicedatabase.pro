TARGET=tst_servicedatabase
QT = core sql
INCLUDEPATH += ../../../src/serviceframework
DEPENDPATH += ../../../src/serviceframework

wince*|symbian*: {
    addFiles.sources = testdata/*
    addFiles.path = testdata
    DEPLOYMENT += addFiles
}

wince* {
    DEFINES+= TESTDATA_DIR=\\\".\\\"
} else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

CONFIG+=testcase

include(../../../common.pri)

# Input 
SOURCES += tst_servicedatabase.cpp \
            
CONFIG+=testcase
CONFIG += mobility
MOBILITY = serviceframework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

wince* {
    DEPLOYMENT_PLUGIN += qsqlite
}
