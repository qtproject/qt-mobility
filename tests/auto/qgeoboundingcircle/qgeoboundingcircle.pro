TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoboundingcircle

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
SOURCES += tst_qgeoboundingcircle.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
