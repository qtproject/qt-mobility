TEMPLATE = app
CONFIG+=testcase
QT += network
TARGET = tst_qnmeapositioninfosource_simulation_generic

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

HEADERS += ../qlocationtestutils_p.h \
           ../testqgeopositioninfosource_p.h \
           qnmeapositioninfosourceproxyfactory.h \
           tst_qnmeapositioninfosource.h

SOURCES += ../qlocationtestutils.cpp \
           ../testqgeopositioninfosource.cpp \
           qnmeapositioninfosourceproxyfactory.cpp \
           tst_qnmeapositioninfosource.cpp \
           tst_qnmeapositioninfosource_simulation_generic.cpp

CONFIG += mobility
MOBILITY = location

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

