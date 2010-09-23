TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlandmarkmanager

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/landmarks

# Input
SOURCES += tst_qlandmarkmanager.cpp

!symbian {
    QT += sql
}
QT += testlib

RESOURCES += data.qrc

CONFIG += mobility
MOBILITY = location

symbian {
        load(data_caging_paths)
        INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
        TARGET.EPOCALLOWDLLDATA = 1
        TARGET.CAPABILITY = ALL -TCB
        LIBS += -leposlmdbmanlib
}

