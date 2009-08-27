TEMPLATE = lib
TARGET = QtLocation
QT = core

include(../common.pri)

symbian {
    LIBS += -llbs
    INCLUDEPATH += \epoc32\include\osextensions
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
HEADERS += s60_QGeoPositionInfoSourceS60.h \
           s60_QMLBackendAO.h \
           s60_NotificationCallback.h
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
SOURCES += s60_QGeoPositionInfoSourceS60.cpp \
           s60_QMLBackendAO.cpp
}
