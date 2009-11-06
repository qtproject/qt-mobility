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
    DEFINES+= TESTDATA_DIR=\\\".\\\"
} else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

CONFIG+=testcase

include(../../../common.pri)

# Input 
SOURCES += tst_servicedatabase.cpp
            
qtAddLibrary(QtServiceFramework)

symbian {
    libBlock = \
        "$${LITERAL_HASH}ifdef WINSCW" \
        "LIBRARY SFWDatabaseManagerServer.lib" \
        "$${LITERAL_HASH}endif"

    MMP_RULES += libBlock

    TARGET.CAPABILITY = ALL -TCB
}

