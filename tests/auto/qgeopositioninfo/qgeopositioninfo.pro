TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeopositioninfo

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qgeopositioninfo.cpp

qtAddLibrary(QtLocation)

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

