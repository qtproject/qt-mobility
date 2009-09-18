TEMPLATE = app
TARGET = tst_qsatelliteinfosource
CONFIG+=testcase

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qsatelliteinfosource.cpp

qtAddLibrary(QtLocation)

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

