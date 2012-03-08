TARGET = tst_CreateDifferentInfosInDifferentThreads
CONFIG+=testcase

SOURCES  += tst_CreateDifferentInfosInDifferentThreads.cpp
QT = core network
INCLUDEPATH += ../../../../../src/systeminfo

include(../../../../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}