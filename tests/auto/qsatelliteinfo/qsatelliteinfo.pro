TEMPLATE = app
CONFIG+=testcase

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qsatelliteinfo.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

