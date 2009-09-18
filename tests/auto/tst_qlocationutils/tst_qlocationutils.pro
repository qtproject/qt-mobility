TEMPLATE = app
TARGET = tst_qlocationutils
CONFIG+=testcase

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qlocationutils.cpp

qtAddLibrary(QtLocation)

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

