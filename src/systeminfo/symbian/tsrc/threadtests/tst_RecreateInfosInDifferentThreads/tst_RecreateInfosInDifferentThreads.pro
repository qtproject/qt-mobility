TARGET = tst_RecreateInfosInDifferentThreads

TARGET = tst_RecreateInfosInDifferentThreads
CONFIG+=testcase

SOURCES  += tst_RecreateInfosInDifferentThreads.cpp
QT = core network
INCLUDEPATH += ../../../../../src/systeminfo

include(../../../../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}