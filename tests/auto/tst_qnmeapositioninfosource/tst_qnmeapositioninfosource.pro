TEMPLATE = app
TARGET = tst_qnmeapositioninfosource
CONFIG+=testcase
QT += network

include (../../../common.pri)

INCLUDEPATH += ../../../location

HEADERS += ../qgeopositioninfosourcesubclasstest_p.h \
           ../qlocationtestutils_p.h

SOURCES += tst_qnmeapositioninfosource.cpp \
           ../qgeopositioninfosourcesubclasstest.cpp \
           ../qlocationtestutils.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

