TARGET = tst_qmediarecorder_xa
INCLUDEPATH += ../../../../src/multimedia \
    ../../../../include
CONFIG += testcase

HEADERS += \
    tst_qmediarecorder_xa.h \
    tst_qmediarecorder_xa_macros.h

SOURCES += \
    tst_qmediarecorder_xa_aac.cpp \
    tst_qmediarecorder_xa_amr.cpp \
    tst_qmediarecorder_xa_main.cpp \
    tst_qmediarecorder_xa_wav.cpp

include (../../../../common.pri)


CONFIG += mobility
MOBILITY = multimedia

#UserEnvironment WriteDeviceData ReadDeviceData MultimediaDD 
symbian {
        TARGET.CAPABILITY = ALL -TCB
}
