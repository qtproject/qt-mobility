TARGET = tst_qdeclarativedeviceinfo
CONFIG+=testcase

QT = core network
QT += declarative

SOURCES  += tst_qdeclarativedeviceinfo.cpp

HEADERS += ../../../plugins/declarative/systeminfo/qdeclarativedeviceinfo_p.h 

SOURCES += ../../../plugins/declarative/systeminfo/qdeclarativedeviceinfo.cpp

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
