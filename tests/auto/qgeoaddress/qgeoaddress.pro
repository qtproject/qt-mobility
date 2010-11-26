TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoaddress


include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/maps

# Input
HEADERS += tst_qgeoaddress.h \
           ../qlocationtestutils_p.h
SOURCES += tst_qgeoaddress.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
