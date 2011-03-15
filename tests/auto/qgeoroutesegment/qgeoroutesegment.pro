TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoroutesegment

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/maps

# Input
HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           tst_qgeoroutesegment.h
SOURCES += tst_qgeoroutesegment.cpp \
           ../qgeocoordinate/qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
