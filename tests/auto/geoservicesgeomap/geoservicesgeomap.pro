TEMPLATE = app
CONFIG+=testcase
TARGET=tst_geoservicesgeomapplugin

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
               ../../../src/location/maps
# Input
HEADERS += ../qlocationtestutils_p.h

SOURCES += tst_geoservicesgeomapplugin.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
