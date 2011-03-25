TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoroutingmanager

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
               ../../../src/location/maps

HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
    tst_qgeoroutingmanager.h

SOURCES += tst_qgeoroutingmanager.cpp \
           ../qgeocoordinate/qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
