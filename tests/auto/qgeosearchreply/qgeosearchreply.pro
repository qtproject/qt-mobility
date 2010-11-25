TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosearchreply

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += ../qlocationtestutils_p.h \
           tst_qgeosearchreply.h
SOURCES += tst_qgeosearchreply.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
