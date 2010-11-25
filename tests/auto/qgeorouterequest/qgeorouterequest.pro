TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeorouterequest

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += tst_qgeorouterequest.h
SOURCES += tst_qgeorouterequest.cpp 

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
