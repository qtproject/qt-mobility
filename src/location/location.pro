TEMPLATE = lib
TARGET = QtLocation
QT = core

include(../../common.pri)

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

INCLUDEPATH += .
DEPENDPATH += .

PUBLIC_HEADERS +=

PUBLIC_HEADERS += qgeocoordinate.h \
                  qgeopositioninfo.h \
                  qgeosatelliteinfo.h \
                  qgeosatelliteinfosource.h \
                  qgeopositioninfosource.h \
                  qgeoareamonitor.h \
                  qnmeapositioninfosource.h

PRIVATE_HEADERS += qlocationutils_p.h \
                   qnmeapositioninfosource_p.h

symbian {
    PRIVATE_HEADERS += qgeopositioninfosource_s60_p.h \
                       qmlbackendao_s60_p.h \
                       qgeosatelliteinfosource_s60_p.h \
                       notificationcallback_s60_p.h \
                       notificationsatellitecallback_s60_p.h

    contains(lbt_enabled, yes) {
        PRIVATE_HEADERS += \
                       qgeoareamonitor_s60_p.h \
                       qmlbackendmonitorao_s60_p.h \
                       qmlbackendmonitorcreatetriggerao_s60_p.h \
                       qmlbackendmonitorinfo_s60_p.h \
                       qmlbackendtriggerchangeao_s60_p.h \
                       notificationmonitorcallback_s60_p.h
    }

    SOURCES += qgeopositioninfosource_s60.cpp \
               qgeosatelliteinfosource_s60.cpp \
               qmlbackendao_s60.cpp

    contains(lbt_enabled, yes) {
        SOURCES += \
               qgeoareamonitor_s60.cpp \
               qmlbackendmonitorao_s60.cpp \
               qmlbackendmonitorcreatetriggerao_s60.cpp \
               qmlbackendmonitorinfo_s60.cpp \
               qmlbackendtriggerchangeao_s60.cpp
    }
}

wince* {
    PRIVATE_HEADERS += qgeopositioninfosource_wince_p.h \
                       qgeosatelliteinfosource_wince_p.h \
                       qgeoinfothread_wince_p.h
    SOURCES += qgeopositioninfosource_wince.cpp \
               qgeosatelliteinfosource_wince.cpp \
               qgeoinfothread_wince.cpp
 
    LIBS += -lgpsapi
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES +=  qlocationutils.cpp \
            qgeocoordinate.cpp \
            qgeopositioninfo.cpp \
            qgeosatelliteinfo.cpp \
            qgeosatelliteinfosource.cpp \
            qgeopositioninfosource.cpp \
            qgeoareamonitor.cpp \
            qnmeapositioninfosource.cpp

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC83

    INCLUDEPATH += $$EPOCROOT\epoc32\include\osextensions \
                   $$EPOCROOT\epoc32\include\LBTHeaders
    LIBS += -llbs
    contains(lbt_enabled, yes) {
        LIBS += -llbt
    }

    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    
    #export headers into EPOCROOT
    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
    }

    QtLocationDeployment.sources = QtLocation.dll
    QtLocationDeployment.path = /sys/bin
    DEPLOYMENT += QtLocationDeployment
}

include(../../features/deploy.pri)
