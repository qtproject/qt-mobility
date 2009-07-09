TEMPLATE = app
TARGET = tst_qgeocoordinate
CONFIG+=testcase

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
HEADERS += ../qlocationtestutils_p.h
SOURCES += tst_qgeocoordinate.cpp \
           ../qlocationtestutils.cpp

LIBS += -lQtLocation

symbian {
    INCLUDEPATH += /epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
