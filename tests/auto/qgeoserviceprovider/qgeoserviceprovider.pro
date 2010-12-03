QT += testlib
TEMPLATE = app
TARGET=tst_qgeoserviceprovider
CONFIG+=testcase

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
               ../../../src/location/maps

HEADERS += ../qlocationtestutils_p.h

SOURCES += tst_qgeoserviceprovider.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
    LIBS           += -lqtgeoservices_staticplugin1.lib
    LIBS           += -lqtgeoservices_staticplugin2.lib
} else {
    LIBS           += -L../../../build/tests/bin/plugins/geoservices
    LIBS           += -l$$mobilityDeployFilename(qtgeoservices_staticplugin1)
    LIBS           += -l$$mobilityDeployFilename(qtgeoservices_staticplugin2)

    TESTDLLS = $$mobilityDeployFilename(qtgeoservices_staticplugin1) \
               $$mobilityDeployFilename(qtgeoservices_staticplugin2)
}
