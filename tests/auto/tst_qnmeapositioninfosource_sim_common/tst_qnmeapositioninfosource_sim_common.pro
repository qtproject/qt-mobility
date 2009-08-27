TEMPLATE = app
TARGET = tst_qnmeapositioninfosource_sim_common
CONFIG+=testcase
QT += network

include (../../../common.pri)

INCLUDEPATH += ../../../location

HEADERS += ../tst_qnmeapositioninfosource_p.h \
           ../qnmeapositioninfosourceproxyfactory.h \
           ../qlocationtestutils_p.h

SOURCES += ../tst_qnmeapositioninfosource.cpp \
           ../qnmeapositioninfosourceproxyfactory.cpp \
           ../qlocationtestutils.cpp \
           tst_qnmeapositioninfosource_sim_common.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}
