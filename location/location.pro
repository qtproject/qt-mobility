TEMPLATE = lib
TARGET = QtLocation
QT = core

include(../common.pri)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

INCLUDEPATH += .
DEPENDPATH += .

HEADERS +=  qlocationglobal.h \
            qcoordinate.h \
            qpositionupdate.h \
            qsatelliteinfo.h

SOURCES +=  qcoordinate.cpp \
            qpositionupdate.cpp \
            qsatelliteinfo.cpp

