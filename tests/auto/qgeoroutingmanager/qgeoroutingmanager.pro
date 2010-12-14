TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoroutingmanager

include (../../../common.pri)

INCLUDEPATH += ../../../src/location \
               ../../../src/location/maps

HEADERS += ../qlocationtestutils_p.h \
    tst_qgeoroutingmanager.h

SOURCES += tst_qgeoroutingmanager.cpp \
           ../qlocationtestutils.cpp

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
