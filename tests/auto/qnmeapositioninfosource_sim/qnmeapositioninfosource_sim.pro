TEMPLATE = app
CONFIG+=testcase
QT += network

include (../../../common.pri)

INCLUDEPATH += ../../../location

HEADERS += ../qgeopositioninfosourcesubclasstest_p.h \
           ../qnmeapositioninfosourceproxyfactory.h \
           ../qlocationtestutils_p.h

SOURCES += ../qgeopositioninfosourcesubclasstest.cpp \
           ../qnmeapositioninfosourceproxyfactory.cpp \
           ../qlocationtestutils.cpp \
           tst_qnmeapositioninfosource_sim.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}
