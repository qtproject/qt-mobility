TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosatelliteinfo

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
SOURCES += tst_qgeosatelliteinfo.cpp

qtAddLibrary(QtLocation)

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

