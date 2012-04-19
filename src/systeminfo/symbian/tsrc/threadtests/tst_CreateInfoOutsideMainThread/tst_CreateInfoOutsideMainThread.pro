TARGET = tst_CreateInfoOutsideMainThread
CONFIG+=testcase

SOURCES  += tst_CreateInfoOutsideMainThread.cpp
QT = core network
INCLUDEPATH += ../../../../../src/systeminfo

include(../../../../../../common.pri)
CONFIG += mobility
MOBILITY = systeminfo

symbian {
    TARGET.CAPABILITY = All -TCB -DRM
}