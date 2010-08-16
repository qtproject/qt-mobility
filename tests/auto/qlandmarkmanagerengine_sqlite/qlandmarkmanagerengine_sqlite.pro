TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlandmarkmanagerengine_sqlite

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input
SOURCES += tst_qlandmarkmanagerengine_sqlite.cpp

QT += sql testlib

RESOURCES += data.qrc

CONFIG += mobility
MOBILITY = location

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

