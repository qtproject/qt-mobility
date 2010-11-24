TEMPLATE = app
CONFIG+=testcase
TARGET=tst_routemanager

include (../../../../common.pri)

INCLUDEPATH += ../../../../src/location \
               ../../../../src/location/maps

HEADERS += ../../qlocationtestutils_p.h \
    tst_qgeoroutemanager.h

SOURCES += tst_qgeoroutemanager.cpp \
           ../../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB

    LIBS           += -lqtgeoservices_staticrouteplugin
} else {
    LIBS           += -L../../../../build/tests/bin/plugins/geoservices -lqtgeoservices_staticrouteplugin
    TESTDLLS = $$mobilityDeployFilename(qtgeoservices_staticrouteplugin)
}
