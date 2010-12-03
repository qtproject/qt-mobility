TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoroutesegment

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
                ../../../src/location/maps

# Input 
HEADERS += ../qlocationtestutils_p.h \
           tst_qgeoroutesegment.h
SOURCES += tst_qgeoroutesegment.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
