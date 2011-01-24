TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosearchmanager

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
               ../../../src/location/maps \
               ../../../src/location/landmarks

HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
    tst_qgeosearchmanager.h

SOURCES += tst_qgeosearchmanager.cpp \
           ../qgeocoordinate/qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
