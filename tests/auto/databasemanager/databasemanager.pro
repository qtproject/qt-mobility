TARGET=tst_databasemanager
QT = core sql
INCLUDEPATH += ../../../src/serviceframework

symbian {
    INCLUDEPATH += ../../../src/serviceframework/symbian
    DEPENDPATH += ../../../src/serviceframework \
                  ../../../src/serviceframework/symbian
}

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
    SOURCES += tst_databasemanager_s60.cpp \
               ../qsfwtestutil.cpp

    HEADERS += \
               ../qsfwtestutil.h

   libBlock = \
       "$${LITERAL_HASH}ifdef WINSCW" \
       "LIBRARY qsfwdatabasemanagerserver.lib" \
       "$${LITERAL_HASH}endif"

    MMP_RULES += libBlock
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
