TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlandmarkmangerengine_sqlite

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
SOURCES += tst_qlandmarkmanagerengine_sqlite.cpp

QT += sql testlib

CONFIG += mobility
MOBILITY = location

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

