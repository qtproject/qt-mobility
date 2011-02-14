TARGET = tst_qdeclarativenetworkinfo
CONFIG+=testcase

QT = core network
QT += declarative

SOURCES  += tst_qdeclarativenetworkinfo.cpp

HEADERS += ../../../plugins/declarative/systeminfo/qdeclarativenetworkinfo_p.h 

SOURCES += ../../../plugins/declarative/systeminfo/qdeclarativenetworkinfo.cpp 

INCLUDEPATH += ../../../src/systeminfo
INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/systeminfo

include(../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}
