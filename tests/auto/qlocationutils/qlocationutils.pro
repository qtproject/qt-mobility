TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlocationutils

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
SOURCES += tst_qlocationutils.cpp

qtAddLibrary(QtLocation)

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

