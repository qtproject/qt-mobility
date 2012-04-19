TARGET = tst_CreateSameInfosInDifferentThreads
CONFIG+=testcase

SOURCES  += tst_CreateSameInfosInDifferentThreads.cpp
QT = core network
INCLUDEPATH += ../../../../../src/systeminfo

include(../../../../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}