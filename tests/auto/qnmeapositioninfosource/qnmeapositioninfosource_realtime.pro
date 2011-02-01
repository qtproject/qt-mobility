TEMPLATE = app
CONFIG+=testcase
QT += network
TARGET = tst_qnmeapositioninfosource_realtime

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           ../qgeopositioninfosource/testqgeopositioninfosource_p.h \
           qnmeapositioninfosourceproxyfactory.h \
           tst_qnmeapositioninfosource.h

SOURCES += ../qgeocoordinate/qlocationtestutils.cpp \
           ../qgeopositioninfosource/testqgeopositioninfosource.cpp \
           qnmeapositioninfosourceproxyfactory.cpp \
           tst_qnmeapositioninfosource.cpp \
           tst_qnmeapositioninfosource_realtime.cpp

CONFIG += mobility
MOBILITY = location

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

