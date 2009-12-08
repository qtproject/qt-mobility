TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeopositioninfosource

include (../../../common.pri)

INCLUDEPATH += ../../../src/location
DEPENDPATH += ../../../buid/Debug/bin

# Input 
HEADERS += ../qlocationtestutils_p.h \
           ../testqgeopositioninfosource_p.h
SOURCES += ../qlocationtestutils.cpp \
           ../testqgeopositioninfosource.cpp \
           tst_qgeopositioninfosource.cpp

CONFIG += mobility
MOBILITY = location

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

