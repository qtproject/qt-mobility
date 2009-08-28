TEMPLATE = app
TARGET = tst_qnmeapositioninfosource
CONFIG+=testcase
QT += network

include (../../../common.pri)

INCLUDEPATH += ../../../location

HEADERS += ../qnmeapositioninfosourceproxyfactory.h \
           ../qlocationtestutils_p.h

SOURCES += ../qnmeapositioninfosourceproxyfactory.cpp \
           ../qlocationtestutils.cpp \
           tst_qnmeapositioninfosource.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}
