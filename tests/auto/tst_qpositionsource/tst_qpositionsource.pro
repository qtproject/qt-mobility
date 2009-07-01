TEMPLATE = app
TARGET = tst_qpositionsource
CONFIG+=testcase

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qpositionsource.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

