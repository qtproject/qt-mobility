TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlandmarkfilehandler_gpx

include (../../../common.pri)

QT += sql

INCLUDEPATH += ../../../src/location

# Input 
SOURCES += tst_qlandmarkfilehandler_gpx.cpp

RESOURCES += data.qrc

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
