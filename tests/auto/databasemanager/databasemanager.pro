TARGET=tst_databasemanager
QT = core sql
INCLUDEPATH += ../../../src/serviceframework

wince*|symbian*: {
    addFiles.sources = testdata/*
    addFiles.path = testdata
    DEPLOYMENT += addFiles
}

wince* {
    DEFINES+= TESTDATA_DIR=\\\".\\\"
    DEPLOYMENT_PLUGIN += qsqlite
} else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

CONFIG+=testcase

include(../../../common.pri)

symbian {
    contains(S60_VERSION, 5.2)|contains(MOBILITY_SD_MCL_BUILD, yes){
        DEFINES += SYMBIAN_EMULATOR_SUPPORTS_PERPROCESS_WSD
    }
    SOURCES += tst_databasemanager_symbian.cpp \
               ../qsfwtestutil.cpp

    TARGET.CAPABILITY = ALL -TCB
    LIBS += -lefsrv

} else {
    DEPENDPATH += ../../../src/serviceframework
    HEADERS += ../qsfwtestutil.h \


    SOURCES += ../qsfwtestutil.cpp \
                    tst_databasemanager.cpp
}

CONFIG += mobility
MOBILITY = serviceframework
