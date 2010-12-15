TARGET = tst_qdeclarativedisplayinfo
CONFIG+=testcase

SOURCES  += tst_qdeclarativedisplayinfo.cpp
QT = core network gui
HEADERS += ../../../plugins/declarative/systeminfo/qdeclarativedisplayinfo_p.h \
             ../../../src/systeminfo/qsystemdisplayinfo.cpp

SOURCES += ../../../plugins/declarative/systeminfo/qdeclarativedisplayinfo.cpp \
           ../../../src/systeminfo/qsystemdisplayinfo.h

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
