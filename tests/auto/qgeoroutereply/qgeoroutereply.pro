TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoroutereply

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += ../qlocationtestutils_p.h \
           tst_qgeoroutereply.h
SOURCES += tst_qgeoroutereply.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
