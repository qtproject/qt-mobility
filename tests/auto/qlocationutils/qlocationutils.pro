TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlocationutils

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qlocationutils.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

