TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeomaneuver

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += ../qlocationtestutils_p.h \
           tst_qgeomaneuver.h
SOURCES += tst_qgeomaneuver.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
