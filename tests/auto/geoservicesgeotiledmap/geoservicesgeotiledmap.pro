TEMPLATE = app
CONFIG+=testcase
TARGET=tst_geoservicesgeotiledmapplugin

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
               ../../../src/location/maps \
               ../../../src/location/maps/tiled

# Input 
HEADERS += ../qlocationtestutils_p.h

SOURCES += tst_geoservicesgeotiledmapplugin.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
    LIBS           += -lqtgeoservices_statictiledgeomapplugin.lib
} else {
    LIBS           += -L../../../build/tests/bin/plugins/geoservices
    LIBS           += -l$$mobilityDeployFilename(qtgeoservices_statictiledgeomapplugin)
    TESTDLLS = $$mobilityDeployFilename(qtgeoservices_statictiledgeomapplugin)
}
