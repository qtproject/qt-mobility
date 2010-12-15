TARGET = tst_qdeclarativesysteminfo
CONFIG+=testcase

SOURCES  += tst_qdeclarativesysteminfo.cpp
QT = core network
QT += declarative

HEADERS += ../../../plugins/declarative/systeminfo/qdeclarativegeneralinfo_p.h

SOURCES += ../../../plugins/declarative/systeminfo/qdeclarativegeneralinfo.cpp

INCLUDEPATH += ../../../src/systeminfo
INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/systeminfo


include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
contains(test_use_sim, yes) {
DEFINES += TESTR QT_SIMULATOR
}
