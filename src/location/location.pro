TEMPLATE = lib
TARGET = QtLocation
QT = core gui network

include(../../common.pri)

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

INCLUDEPATH += .
DEPENDPATH += .

PUBLIC_HEADERS += qgeoaddress.h \
                  qgeolocation.h \
                  qgeocoordinate.h \
                  qgeopositioninfo.h \
                  qgeosatelliteinfo.h \
                  qgeosatelliteinfosource.h \
                  qgeopositioninfosource.h \
                  qgeoareamonitor.h \
                  qnmeapositioninfosource.h \
                  qlandmark.h \
                  qlandmark_p.h \
                  qlandmarkid.h \
                  qlandmarkid_p.h \
                  qlandmarkmanager.h \
                  qlandmarkfilter.h \
                  qlandmarkabstractrequest.h \
                  qlandmarkidfetchrequest.h \
                  qlandmarkfetchrequest.h \
                  qlandmarksaverequest.h \
                  qlandmarkremoverequest.h \
                  qlandmarkimportrequest.h \
                  qlandmarkexportrequest.h \
                  qlandmarkcategoryidfetchrequest.h \
                  qlandmarkcategoryfetchrequest.h \
                  qlandmarkcategorysaverequest.h \
                  qlandmarkcategoryremoverequest.h \
                  qlandmarkboxfilter.h \
                  qlandmarkcategoryid.h \
                  qlandmarkcategoryid_p.h \
                  qlandmarkcategory_p.h \
                  qlandmarkcategory.h \
                  qlandmarknamefilter.h \
                  qlandmarkproximityfilter.h \
                  qlandmarknearestfilter.h \
                  qlandmarkcategoryfilter.h \
                  qlandmarkintersectionfilter.h \
                  qlandmarkunionfilter.h \
                  qlandmarkcustomfilter.h \
                  qlandmarksortorder.h \
                  qlandmarknamesort.h \
                  qlandmarkdistancesort.h \
                  qlandmarkcustomsort.h \
                  qlandmarkmanagerengine.h \
                  qlandmarkmanagerenginefactory.h

PRIVATE_HEADERS += qlocationutils_p.h \
                   qnmeapositioninfosource_p.h \
                   qlandmark_p.h \
                   qgeolocation_p.h

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

maemo6 {
    CONFIG += qdbus link_pkgconfig
    SOURCES += qgeopositioninfosource_maemo.cpp \
                qgeosatelliteinfosource_maemo.cpp \
                dbuscomm_maemo.cpp \
                dbusserver_maemo.cpp
    HEADERS += qgeopositioninfosource_maemo_p.h \
                qgeosatelliteinfosource_maemo_p.h \
                dbuscomm_maemo_p.h \
                dbusserver_maemo_p.h
}

maemo5 {
    CONFIG += qdbus link_pkgconfig
    SOURCES += gconfitem.cpp \
               liblocationwrapper.cpp \
               qgeopositioninfosource_maemo5.cpp \
               qgeosatelliteinfosource_maemo5.cpp \
               qgeoareamonitor_maemo.cpp
    HEADERS += gconfitem.h  \
               liblocationwrapper.h \
               qgeopositioninfosource_maemo5_p.h \
               qgeosatelliteinfosource_maemo5_p.h \
               qgeoareamonitor_maemo_p.h
    PKGCONFIG += glib-2.0  gconf-2.0
    CONFIG += create_pc create_prl
    LIBS += -llocation
    QMAKE_PKGCONFIG_REQUIRES = glib-2.0 gconf-2.0
    pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
    pkgconfig.files = QtLocation.pc
}

# maps header files - sort into public and private later
HEADERS += qgeocodingreply.h \
           qgeocodingreply_p.h \
           qgeocodingrequest.h \
           qgeocodingrequest_p.h \
           qgeocodingxmlparser.h \
           qgeoengine.h \
           qgeonetworkmanager.h \
           qgeonetworkmanager_p.h \
           qgeoreply.h \
           qmaneuver.h \
           qmapellipse.h \
           qmapellipse_p.h \
           qmapline.h \
           qmapline_p.h \
           qmapmarker.h \
           qmapmarker_p.h \
           qmapobject.h \
           qmapobject_p.h \
           qmappixmap.h \
           qmappixmap_p.h \
           qmappolygon.h \
           qmappolygon_p.h \
           qmaprect.h \
           qmaprect_p.h\
           qmaproute.h \
           qmaproute_p.h \
           qmaptile.h \
           qmaptilecache.h \
           qmaptilereply.h \
           qmaptilerequest.h \
           qmaptilerequest_p.h \
           qmapview.h \
           qmapview_p.h \
           qreversegeocodingrequest.h \
           qreversegeocodingrequest_p.h \
           qroute.h \
           qroutereply.h \
           qroutereply_p.h \
           qrouterequest.h \
           qrouterequest_p.h \
           qroutexmlparser.h \
           qlandmarkfilter_p.h \
           qgeoaddress_p.h

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += qlocationutils.cpp \
           qgeocoordinate.cpp \
           qgeopositioninfo.cpp \
           qgeosatelliteinfo.cpp \
           qgeosatelliteinfosource.cpp \
           qgeopositioninfosource.cpp \
           qgeoareamonitor.cpp \
           qnmeapositioninfosource.cpp \
           qlandmarkid.cpp \
           qlandmark.cpp \
           qlandmarkmanager.cpp \
           qlandmarkfilter.cpp \
           qlandmarkabstractrequest.cpp \
           qlandmarkidfetchrequest.cpp \
           qlandmarkfetchrequest.cpp \
           qlandmarksaverequest.cpp \
           qlandmarkremoverequest.cpp \
           qlandmarkimportrequest.cpp \
           qlandmarkexportrequest.cpp \
           qlandmarkcategoryidfetchrequest.cpp \
           qlandmarkcategoryfetchrequest.cpp \
           qlandmarkcategorysaverequest.cpp \
           qlandmarkcategoryremoverequest.cpp \
           qlandmarkboxfilter.cpp \
           qlandmarkcategoryid.cpp \
           qlandmarkcategory.cpp \
           qlandmarknamefilter.cpp \
           qlandmarkproximityfilter.cpp \
           qlandmarknearestfilter.cpp \
           qlandmarkcategoryfilter.cpp \
           qlandmarkintersectionfilter.cpp \
           qlandmarkunionfilter.cpp \
           qlandmarkcustomfilter.cpp \
           qlandmarksortorder.cpp \
           qlandmarknamesort.cpp \
           qlandmarkdistancesort.cpp \
           qlandmarkcustomsort.cpp \
           qlandmarkmanagerengine.cpp \
           qlandmarkmanagerenginefactory.cpp

# maps source files
SOURCES += qgeoaddress.cpp \
           qgeocodingreply.cpp \
           qgeocodingrequest.cpp \
           qgeocodingxmlparser.cpp \
           qgeoengine.cpp \
           qgeolocation.cpp \
           qgeonetworkmanager.cpp \
           qgeoreply.cpp \
           qmaneuver.cpp \
           qmapobject.cpp \
           qmapline.cpp \
           qmapmarker.cpp \
           qmaproute.cpp \
           qmaprect.cpp \
           qmappolygon.cpp \
           qmappixmap.cpp \
           qmapellipse.cpp \
           qmaptilerequest.cpp \
           qmaptilecache.cpp \
           qmaptilereply.cpp \
           qmapview.cpp \
           qmapview_p.cpp \
           qreversegeocodingrequest.cpp \
           qroute.cpp \
           qroutereply.cpp \
           qrouterequest.cpp \
            qroutexmlparser.cpp

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC83

    INCLUDEPATH += $${EPOCROOT}epoc32\include\osextensions \
                   $${EPOCROOT}epoc32\include\LBTHeaders
    LIBS += -llbs
    contains(lbt_enabled, yes) {
        LIBS += -llbt
    }

    QtLocationDeployment.sources = QtLocation.dll
    QtLocationDeployment.path = /sys/bin
    DEPLOYMENT += QtLocationDeployment
}

CONFIG += middleware
include(../../features/deploy.pri)
