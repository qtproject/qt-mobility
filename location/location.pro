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

symbian {
    HEADERS += s60_QGeoPositionInfoSourceS60.h \
           s60_QMLBackendAO.h \
           s60_NotificationCallback.h
    SOURCES += s60_QGeoPositionInfoSourceS60.cpp \
           s60_QMLBackendAO.cpp
}

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
    INCLUDEPATH += \epoc32\include\osextensions
    LIBS += -llbs

    deploy.path = /
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    DEPLOYMENT += exportheaders
}

