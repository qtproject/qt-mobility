TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoroutingmanager

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
               ../../../src/location/maps

HEADERS += ../qlocationtestutils_p.h \
    tst_qgeoroutingmanager.h

SOURCES += tst_qgeoroutingmanager.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB

    LIBS           += -lqtgeoservices_staticroutingplugin
} else {
    LIBS           += -L../../../build/tests/bin/plugins/geoservices
    LIBS           += -l$$mobilityDeployFilename(qtgeoservices_staticroutingplugin)
    TESTDLLS = $$mobilityDeployFilename(qtgeoservices_staticroutingplugin)
}
