TARGET = tst_qmediarecorder
INCLUDEPATH += ../../../src/multimedia ../../../src/multimedia/audio
CONFIG += testcase

symbian {
    include(../../../config.pri)
    HEADERS += s60common.h
    HEADERS += tst_qmediarecorder_xa.h
    SOURCES += tst_qmediarecorder_xa.cpp
    HEADERS += tst_qmediarecorder_mmf.h
    SOURCES += tst_qmediarecorder_mmf.cpp
    TARGET.CAPABILITY = UserEnvironment ReadDeviceData WriteDeviceData AllFiles
}

HEADERS += tst_qmediarecorder.h

SOURCES += main.cpp tst_qmediarecorder.cpp

include (../../../common.pri)
include (../qmultimedia_common/mock.pri)
include (../qmultimedia_common/mockrecorder.pri)

CONFIG += mobility
MOBILITY = multimedia

