TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlandmarkfilehandler_lmx

include (../../../common.pri)

QT += sql

INCLUDEPATH += ../../../src/location

# Input 
SOURCES += tst_qlandmarkfilehandler_lmx.cpp

RESOURCES += data.qrc

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
