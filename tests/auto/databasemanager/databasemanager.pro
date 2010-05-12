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
    SOURCES += tst_databasemanager_symbian.cpp \
               ../qsfwtestutil.cpp

    HEADERS += \
               ../qsfwtestutil.h

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
