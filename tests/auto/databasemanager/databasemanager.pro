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
    DEFINES+= SRCDIR=\\\".\\\"
} !symbian {
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

    HEADERS += databasemanager_s60.h \
               databasemanager_s60.h \
               dberror.h \
               ../qsfwtestutil.h
               
   libBlock = \
       "$${LITERAL_HASH}ifdef WINSCW" \
       "LIBRARY SFWDatabaseManagerServer.lib" \
       "$${LITERAL_HASH}endif"
               
    MMP_RULES += libBlock
    TARGET.CAPABILITY = ALL -TCB
    LIBS += -lefsrv

} else {
	HEADERS += ../qsfwtestutil.h
	SOURCES += ../qsfwtestutil.cpp \
               tst_databasemanager.cpp
}

LIBS += -lQtServiceFramework
