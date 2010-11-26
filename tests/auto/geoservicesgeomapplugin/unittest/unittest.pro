TEMPLATE = app
CONFIG+=testcase
TARGET=tst_geoservicesgeomapplugin

include (../../../../common.pri)

INCLUDEPATH += ../../../../src/location \
               ../../../../src/location/maps
# Input
HEADERS += ../../qlocationtestutils_p.h

SOURCES += tst_geoservicesgeomapplugin.cpp \
           ../../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
    LIBS           += -lqtgeoservices_staticgeomapplugin.lib
} else {
    LIBS           += -L../../../../build/tests/bin/plugins/geoservices -lqtgeoservices_staticgeomapplugin
    TESTDLLS = $$mobilityDeployFilename(qtgeoservices_staticgeomapplugin)
}
