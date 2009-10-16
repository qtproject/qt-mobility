TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosatelliteinfo

include (../../../common.pri)

INCLUDEPATH += ../../../location

# Input 
SOURCES += tst_qgeosatelliteinfo.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

