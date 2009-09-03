TEMPLATE = lib
TARGET = QtLocation
QT = core

include(../common.pri)

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

INCLUDEPATH += .
DEPENDPATH += .

PUBLIC_HEADERS +=

PUBLIC_HEADERS += qlocationglobal.h \
                  qgeocoordinate.h \
                  qgeopositioninfo.h \
                  qsatelliteinfo.h \
                  qsatelliteinfosource.h \
                  qgeopositioninfosource.h \
                  qgeoareamonitor.h \
                  qnmeapositioninfosource.h

PRIVATE_HEADERS += qlocationutils_p.h \
                   qnmeapositioninfosource_p.h

symbian {
    PRIVATE_HEADERS += qgeopositioninfosource_s60_p.h \
                       qmlbackendao_s60_p.h \
                       notificationcallback_s60_p.h

    SOURCES += qgeopositioninfosource_s60.cpp \
               qmlbackendao_s60.cpp
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES +=  qlocationutils.cpp \
            qgeocoordinate.cpp \
            qgeopositioninfo.cpp \
            qsatelliteinfo.cpp \
            qsatelliteinfosource.cpp \
            qgeopositioninfosource.cpp \
            qgeoareamonitor.cpp \
            qnmeapositioninfosource.cpp

headers.files = $$PUBLIC_HEADERS
headers.path = $$QT_MOBILITY_INCLUDE
INSTALLS+=headers

symbian {
    TARGET.CAPABILITY = ALL -TCB
    INCLUDEPATH += \epoc32\include\osextensions
    LIBS += -llbs

    deploy.path = /
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    DEPLOYMENT += exportheaders
}

