TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoaddress

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += tst_qgeoaddress.h
SOURCES += tst_qgeoaddress.cpp 

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
