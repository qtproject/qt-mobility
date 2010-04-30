TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeopositioninfo

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
SOURCES += tst_qgeopositioninfo.cpp

CONFIG += mobility
MOBILITY = location

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

