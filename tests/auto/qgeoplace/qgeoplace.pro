TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoplace

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += tst_qgeoplace.h
SOURCES += tst_qgeoplace.cpp 

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
