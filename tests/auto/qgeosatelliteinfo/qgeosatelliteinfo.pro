TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosatelliteinfo

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
SOURCES += tst_qgeosatelliteinfo.cpp

CONFIG += mobility
MOBILITY = location

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

