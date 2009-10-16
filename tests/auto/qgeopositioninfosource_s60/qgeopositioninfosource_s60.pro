TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeopositioninfosource_s60

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
HEADERS += ../qlocationtestutils_p.h
SOURCES += ../qlocationtestutils.cpp \
           tst_qgeopositioninfosource_s60.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

