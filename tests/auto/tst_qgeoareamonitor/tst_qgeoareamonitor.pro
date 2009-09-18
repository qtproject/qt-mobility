TEMPLATE = app
TARGET = tst_qgeoareamonitor
CONFIG+=testcase

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qgeoareamonitor.cpp

qtAddLibrary(QtLocation)

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

