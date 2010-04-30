TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoareamonitor

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += ../qlocationtestutils_p.h
SOURCES += tst_qgeoareamonitor.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

