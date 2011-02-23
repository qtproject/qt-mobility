TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosearchmanager

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
               ../../../src/location/maps \
               ../../../src/location/landmarks

HEADERS += ../qlocationtestutils_p.h \
    tst_qgeosearchmanager.h

SOURCES += tst_qgeosearchmanager.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
