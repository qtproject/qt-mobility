TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeocoordinate

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += ../qlocationtestutils_p.h
SOURCES += tst_qgeocoordinate.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
