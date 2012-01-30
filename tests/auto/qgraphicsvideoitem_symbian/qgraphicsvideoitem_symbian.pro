!symbian: error("This test should only be built on Symbian")
TARGET = tst_qgraphicsvideoitem_symbian
CONFIG += testcase mobility
MOBILITY = multimedia
INCLUDEPATH += ../../../src/multimedia
SOURCES += tst_qgraphicsvideoitem_symbian.cpp
LIBS += -lcone -lavkon
include (../../../common.pri)

