TEMPLATE = app
CONFIG+=testcase
QT += network
TARGET = tst_dummynmeapositioninfosource

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           ../qgeopositioninfosource/testqgeopositioninfosource_p.h \
           qnmeapositioninfosourceproxyfactory.h

SOURCES += ../qgeocoordinate/qlocationtestutils.cpp \
           ../qgeopositioninfosource/testqgeopositioninfosource.cpp \
           qnmeapositioninfosourceproxyfactory.cpp \
           tst_dummynmeapositioninfosource.cpp

CONFIG += mobility
MOBILITY = location

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

