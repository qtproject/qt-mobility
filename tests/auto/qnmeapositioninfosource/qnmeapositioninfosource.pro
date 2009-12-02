TEMPLATE = app
CONFIG+=testcase
QT += network
TARGET = tst_qnmeapositioninfosource

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

HEADERS += ../qlocationtestutils_p.h \
           ../testqgeopositioninfosource_p.h \
           qnmeapositioninfosourceproxyfactory.h

SOURCES += ../qlocationtestutils.cpp \
           ../testqgeopositioninfosource.cpp \
           qnmeapositioninfosourceproxyfactory.cpp \
           tst_qnmeapositioninfosource.cpp

CONFIG += mobility
MOBILITY = location

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

