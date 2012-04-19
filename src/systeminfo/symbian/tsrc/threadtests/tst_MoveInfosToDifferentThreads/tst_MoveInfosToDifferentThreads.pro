TARGET = tst_MoveInfosToDifferentThreads
CONFIG+=testcase

SOURCES  += tst_MoveInfosToDifferentThreads.cpp
QT = core network
INCLUDEPATH += ../../../../../src/systeminfo

include(../../../../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}