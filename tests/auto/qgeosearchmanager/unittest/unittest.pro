TEMPLATE = app
CONFIG+=testcase
TARGET=tst_searchmanager

include (../../../../common.pri)

INCLUDEPATH += ../../../../src/location \
               ../../../../src/location/maps

HEADERS += ../../qlocationtestutils_p.h \
    tst_qgeosearchmanager.h

SOURCES += tst_qgeosearchmanager.cpp \
           ../../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB

    LIBS           += -lqtgeoservices_staticsearchplugin
} else {
    LIBS           += -L../../../../build/tests/bin/plugins/geoservices -lqtgeoservices_staticsearchplugin
    TESTDLLS = $$mobilityDeployFilename(qtgeoservices_staticsearchplugin)
}
