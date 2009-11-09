TARGET=tst_databasemanager
QT = core sql
INCLUDEPATH += ../../../serviceframework

symbian {
    INCLUDEPATH += ../../../serviceframework/symbian
    DEPENDPATH += ../../../serviceframework \
                  ../../../serviceframework/symbian
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
                    servicemetadata.cpp \
                    databasemanager_s60.cpp \
                    dberror.cpp \
                    ../qsfwtestutil.cpp

    HEADERS += databasemanager_s60_p.h \
               dberror_p.h \
               ../qsfwtestutil.h

   libBlock = \
       "$${LITERAL_HASH}ifdef WINSCW" \
       "LIBRARY SFWDatabaseManagerServer.lib" \
       "$${LITERAL_HASH}endif"

    MMP_RULES += libBlock
    TARGET.CAPABILITY = ALL -TCB
    LIBS += -lefsrv

} else {
    DEPENDPATH += ../../../serviceframework
    HEADERS += ../qsfwtestutil.h \


    SOURCES += ../qsfwtestutil.cpp \
                    tst_databasemanager.cpp
}

qtAddLibrary(QtServiceFramework)
