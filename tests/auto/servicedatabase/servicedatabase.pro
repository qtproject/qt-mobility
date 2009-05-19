QT = core sql
INCLUDEPATH += ../../../servicefw/serviceframework/servicemetadata \
               ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework/servicedatabase \
               ../../../servicefw/serviceframework

DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
CONFIG+=testcase

include(../../../common.pri)

# Input 
SOURCES += tst_servicedatabasetest.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

