TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoroute

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/maps

# Input
HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           tst_qgeoroute.h
SOURCES += tst_qgeoroute.cpp \
           ../qgeocoordinate/qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
