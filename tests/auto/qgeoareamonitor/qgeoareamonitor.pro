TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoareamonitor

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
HEADERS += ../qlocationtestutils_p.h
SOURCES += tst_qgeoareamonitor.cpp \
           ../qlocationtestutils.cpp

qtAddLibrary(QtLocation)

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

