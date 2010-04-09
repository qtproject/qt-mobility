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
    INCLUDEPATH += ../../../src/serviceframework/symbian
    DEPENDPATH += ../../../src/serviceframework \
                  ../../../src/serviceframework/symbian
    libBlock = \
        "$${LITERAL_HASH}ifdef WINSCW" \
        "LIBRARY SFWDatabaseManagerServer.lib" \
        "$${LITERAL_HASH}endif"

    # DEFINES+= QT_NODLL
    DEFINES+= QT_MAKEDLL
    DEFINES += QT_SFW_SERVICEDATABASE_USE_SECURITY_TOKEN

    MMP_RULES += libBlock
    SOURCES += servicedatabase.cpp
    HEADERS += servicedatabase_p.h
    TARGET.CAPABILITY = ALL -TCB
}

wince* {
    DEPLOYMENT_PLUGIN += qsqlite
}
