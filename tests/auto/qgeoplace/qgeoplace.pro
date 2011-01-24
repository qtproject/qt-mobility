TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoplace

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input
HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           tst_qgeoplace.h
SOURCES += tst_qgeoplace.cpp \
           ../qgeocoordinate/qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
