TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoboundingbox

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
SOURCES += tst_qgeoboundingbox.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
