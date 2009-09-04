TEMPLATE = app
CONFIG+=testcase
QT += network

include (../../../common.pri)

INCLUDEPATH += ../../../location

HEADERS += ../qnmeapositioninfosourceproxyfactory.h \
           ../qgeopositioninfosourcesubclasstest_p.h \
           ../qlocationtestutils_p.h

SOURCES += ../qnmeapositioninfosourceproxyfactory.cpp \
           ../qgeopositioninfosourcesubclasstest.cpp \
           ../qlocationtestutils.cpp \
           tst_qnmeapositioninfosource.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}
