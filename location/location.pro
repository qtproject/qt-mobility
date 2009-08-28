TEMPLATE = lib
TARGET = QtLocation
QT = core

include(../common.pri)

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

INCLUDEPATH += .
DEPENDPATH += .

PUBLIC_HEADERS +=

PUBLIC_HEADERS +=  qlocationglobal.h \
            qgeocoordinate.h \
            qgeopositioninfo.h \
            qsatelliteinfo.h \
            qsatelliteinfosource.h \
            qgeopositioninfosource.h \
            qgeoareamonitor.h \
            qnmeapositioninfosource.h

PRIVATE_HEADERS += qlocationutils_p.h \
            qnmeapositioninfosource_p.h

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES +=  qlocationutils.cpp \
            qgeocoordinate.cpp \
            qgeopositioninfo.cpp \
            qsatelliteinfo.cpp \
            qsatelliteinfosource.cpp \
            qgeopositioninfosource.cpp \
            qgeoareamonitor.cpp \
            qnmeapositioninfosource.cpp

symbian {
    TARGET.CAPABILITY = ALL -TCB

    deploy.path = /
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    DEPLOYMENT += exportheaders
}


