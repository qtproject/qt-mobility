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
            qlocationutils_p.h \
            qcoordinate.h \
            qpositionupdate.h \
            qsatelliteinfo.h \
            qsatelliteinfosource.h \
            qpositionsource.h \
            qpositionareamonitor.h

SOURCES +=  qlocationutils.cpp \
            qcoordinate.cpp \
            qpositionupdate.cpp \
            qsatelliteinfo.cpp \
            qsatelliteinfosource.cpp \
            qpositionsource.cpp \
            qpositionareamonitor.cpp

