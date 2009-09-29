TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosatelliteinfosource

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qgeosatelliteinfosource.cpp

qtAddLibrary(QtLocation)

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

