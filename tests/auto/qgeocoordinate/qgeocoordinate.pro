TEMPLATE = app
CONFIG+=testcase

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
HEADERS += ../qlocationtestutils_p.h
SOURCES += tst_qgeocoordinate.cpp \
           ../qlocationtestutils.cpp

LIBS += -lQtLocation

symbian {
    INCLUDEPATH += $${EPOCROOT}/epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
