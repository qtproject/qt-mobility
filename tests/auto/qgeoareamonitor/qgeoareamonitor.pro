TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeoareamonitor

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qgeoareamonitor.cpp

qtAddLibrary(QtLocation)

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

