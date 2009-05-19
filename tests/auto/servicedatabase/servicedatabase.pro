QT -= gui
QT+=testlib 
TEMPLATE = app
TARGET = tst_servicedatabasetest
INCLUDEPATH += ../../../servicefw/serviceframework/servicemetadata \
               ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework/servicedatabase \
               ../../../servicefw/serviceframework

DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
CONFIG+=testcase

QT += sql

include(../../../common.pri)

# Input 
SOURCES += tst_servicedatabasetest.cpp

LIBS += -L$$DESTDIR -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

