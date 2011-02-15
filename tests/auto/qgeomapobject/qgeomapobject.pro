TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeomapobject

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/maps

# Input
SOURCES += tst_qgeomapobject.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}

