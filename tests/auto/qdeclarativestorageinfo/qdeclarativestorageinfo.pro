HEADERS  += 
TARGET = tst_qdeclarativestorageinfo
CONFIG+=testcase

QT = core network
QT += declarative

SOURCES  += tst_qdeclarativestorageinfo.cpp

HEADERS += ../../../plugins/declarative/systeminfo/qdeclarativestorageinfo_p.h \
             ../../../src/systeminfo/qsystemstorageinfo.cpp

SOURCES += ../../../plugins/declarative/systeminfo/qdeclarativestorageinfo.cpp \
           ../../../src/systeminfo/qsystemstorageinfo.h

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
