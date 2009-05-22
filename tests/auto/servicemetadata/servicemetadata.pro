TARGET=tst_servicemetadata
INCLUDEPATH += ../../../servicefw/serviceframework/servicemetadata \
               ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework

DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
CONFIG+=testcase

QT = core

include(../../../common.pri)

# Input 
SOURCES += tst_servicemetadata.cpp

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
