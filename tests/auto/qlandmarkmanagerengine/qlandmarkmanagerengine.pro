TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlandmarkmanagerengine

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/landmarks

# Input 
SOURCES += tst_qlandmarkmanagerengine.cpp

QT += testlib

CONFIG += mobility
MOBILITY = location

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

