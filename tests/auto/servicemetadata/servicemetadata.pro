TEMPLATE = app
TARGET = tst_servicemetadata
INCLUDEPATH += ../../../servicefw/serviceframework/servicemetadata \
               ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework

DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
CONFIG+=testcase

QT -= gui
QT+=testlib 

include(../../../common.pri)

# Input 
SOURCES += tst_servicemetadata.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = ALL -TCB
}
