TEMPLATE = app
CONFIG+=testcase

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qgeopositioninfo.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

