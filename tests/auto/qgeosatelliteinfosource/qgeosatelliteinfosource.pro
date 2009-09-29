TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosatelliteinfosource

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qgeosatelliteinfosource.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

