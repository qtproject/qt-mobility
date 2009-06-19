TARGET=tst_databasemanager
QT = core sql
INCLUDEPATH += ../../../servicefw/serviceframework/servicemetadata \
               ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework/servicedatabase \
               ../../../servicefw/serviceframework

DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
CONFIG+=testcase

include(../../../common.pri)

HEADERS += ../qsfwtestutil.h
SOURCES += tst_databasemanagertest.cpp \
           ../qsfwtestutil.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

