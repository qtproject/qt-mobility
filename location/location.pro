TEMPLATE = lib
TARGET = QtLocation
QT = core

include(../common.pri)

symbian {
    TARGET.UID3 = 0xEEA0E2D0
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
            qgeoareamonitor.h \
            qnmeapositioninfosource.h \
            qnmeapositioninfosource_p.h

symbian {
HEADERS += s60/QGeoPositionInfoSourceS60.h \
           s60/QMLBackendAO.h \
           s60/NotificationCallback.h
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
SOURCES += s60/QGeoPositionInfoSourceS60.cpp \
           s60/QMLBackendAO.cpp
}