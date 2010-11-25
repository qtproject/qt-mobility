TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosearchreply

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += tst_qgeosearchreply.h
SOURCES += tst_qgeosearchreply.cpp 

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
