TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoroutereply

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += tst_qgeoroutereply.h
SOURCES += tst_qgeoroutereply.cpp 

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
