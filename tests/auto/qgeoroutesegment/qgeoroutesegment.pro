TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoroutesegment

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += tst_qgeoroutesegment.h
SOURCES += tst_qgeoroutesegment.cpp 

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
