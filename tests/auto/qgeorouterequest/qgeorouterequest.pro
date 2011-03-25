TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeorouterequest

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/maps

# Input
HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           tst_qgeorouterequest.h
SOURCES += tst_qgeorouterequest.cpp \
           ../qgeocoordinate/qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
