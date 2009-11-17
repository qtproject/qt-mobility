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

qtAddLibrary(QtLocation)

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

