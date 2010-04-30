TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlocationutils

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
SOURCES += tst_qlocationutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

