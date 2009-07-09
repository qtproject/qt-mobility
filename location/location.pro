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
            qgeocoordinate.h \
            qgeopositioninfo.h \
            qsatelliteinfo.h \
            qsatelliteinfosource.h \
            qgeopositioninfosource.h \
            qgeoareamonitor.h

SOURCES +=  qlocationutils.cpp \
            qgeocoordinate.cpp \
            qgeopositioninfo.cpp \
            qsatelliteinfo.cpp \
            qsatelliteinfosource.cpp \
            qgeopositioninfosource.cpp \
            qgeoareamonitor.cpp

