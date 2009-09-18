TEMPLATE = app
TARGET = tst_qgeopositioninfosource
CONFIG+=testcase

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qgeopositioninfosource.cpp

qtAddLibrary(QtLocation)

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

