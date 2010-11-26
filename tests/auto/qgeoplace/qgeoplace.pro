TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoplace

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/maps

# Input
HEADERS += ../qlocationtestutils_p.h \
           tst_qgeoplace.h
SOURCES += tst_qgeoplace.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
