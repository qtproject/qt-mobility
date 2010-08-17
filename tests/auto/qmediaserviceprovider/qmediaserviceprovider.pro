TARGET = tst_qmediaserviceprovider
INCLUDEPATH += ../../../src/multimedia ../../../src ../../../src/multimedia/audio
CONFIG += testcase

SOURCES += tst_qmediaserviceprovider.cpp

include (../../../common.pri)

CONFIG += mobility
MOBILITY = multimedia
