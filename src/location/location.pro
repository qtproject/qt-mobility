TEMPLATE = lib
TARGET = QtLocation
QT = core gui network

include(../../common.pri)

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

INCLUDEPATH += .
DEPENDPATH += .

PUBLIC_HEADERS += \
                    qgeoaddress.h \
                    qgeoareamonitor.h \
                    qsearchresponse.h \
                    qsearchrequest.h \
                    qsearchrequest_nokia.h \
                    qgeocodingservice.h \
                    qsearchcontroller.h \
                    qgeocoordinate.h \
                    qgeolocation.h \
                    qgeopositioninfo.h \
                    qgeopositioninfosource.h \
                    qgeoroute.h \
                    qgeoroutereply.h \
                    qgeorouterequest.h \
                    qgeoroutingservice.h \
                    qgeosatelliteinfo.h \
                    qgeosatelliteinfosource.h \
                    qlandmark.h \
                    qlandmarkabstractrequest.h \
                    qlandmarkboxfilter.h \
                    qlandmarkcategory.h \
                    qlandmarkcategoryfetchrequest.h \
                    qlandmarkcategoryfilter.h \
                    qlandmarkcategoryid.h \
                    qlandmarkcategoryidfetchrequest.h \
                    qlandmarkcategoryremoverequest.h \
                    qlandmarkcategorysaverequest.h \
                    qlandmarkcustomfilter.h \
                    qlandmarkcustomsort.h \
                    qlandmarkdistancesort.h \
                    qlandmarkexportrequest.h \
                    qlandmarkfetchrequest.h \
                    qlandmarkfilter.h \
                    qlandmarkid.h \
                    qlandmarkidfetchrequest.h \
                    qlandmarkimportrequest.h \
                    qlandmarkintersectionfilter.h \
                    qlandmarkmanager.h \
                    qlandmarkmanagerengine.h \
                    qlandmarkmanagerenginefactory.h \
                    qlandmarknamefilter.h \
                    qlandmarknamesort.h \
                    qlandmarknearestfilter.h \
                    qlandmarkproximityfilter.h \
                    qlandmarkremoverequest.h \
                    qlandmarksaverequest.h \
                    qlandmarksortorder.h \
                    qlandmarkunionfilter.h \
                    qmaneuver.h \
                    qmapellipse.h \
                    qmapline.h \
                    qmapmarker.h \
                    qmapobject.h \
                    qmappixmap.h \
                    qmappolygon.h \
                    qmaprect.h \
                    qmaproute.h \
                    qmaptilereply.h \
                    qmaptileservice.h \
                    qmapview.h \
                    qmapcontainer.h \
                    qgeoboundingbox.h \
                    qnmeapositioninfosource.h 

PRIVATE_HEADERS += \
                    qgeoaddress_p.h \
                    qsearchresponse_nokia_p.h \
                    qsearchresponse_p.h \
                    qsearchrequest_nokia_p.h \
                    qgeocodingservice_nokia_p.h \
                    qsearchcontroller_nokia_p.h \
                    qgeocodingxmlparser_nokia_p.h \
                    qgeolocation_p.h \
                    qgeolocation_p.h \
                    qgeoroute_p.h \
                    qgeoroutereply_nokia_p.h \
                    qgeoroutereply_p.h \
                    qgeorouterequest_p.h \
                    qgeoroutexmlparser_nokia_p.h \
                    qgeoroutingservice_nokia_p.h \
                    qlandmark_p.h \
                    qlandmark_p.h \
                    qlandmarkcategory_p.h \
                    qlandmarkcategoryid_p.h \
                    qlandmarkfilter_p.h \
                    qlandmarkfilter_p.h \
                    qlandmarkid_p.h \
                    qlandmarkmanager_p.h \
                    qlandmarkabstractrequest_p.h \
                    qlocationutils_p.h \
                    qmaneuver_p.h \
                    qmapellipse_p.h \
                    qmapline_p.h \
                    qmapmarker_p.h \
                    qmapobject_p.h \
                    qmappixmap_p.h \
                    qmappolygon_p.h \
                    qmaprect_p.h\
                    qmaproute_p.h \
                    qmaptilecache_nokia_p.h \
                    qmaptilereply_nokia_p.h \
                    qmaptilereply_p.h \
                    qmaptileservice_nokia_p.h \
                    qmapview_p.h \
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

maemo6 {
    CONFIG += qdbus link_pkgconfig
    SOURCES += qgeopositioninfosource_maemo.cpp \
                qgeosatelliteinfosource_maemo.cpp \
                dbuscomm_maemo.cpp \
                dbusserver_maemo.cpp
    PRIVATE_HEADERS += qgeopositioninfosource_maemo_p.h \
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
    PRIVATE_HEADERS += gconfitem.h  \
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
HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += \
            qgeoaddress.cpp \
            qgeoareamonitor.cpp \
            qsearchresponse.cpp \
            qsearchresponse_nokia.cpp \
            qsearchrequest_nokia.cpp \
            qgeocodingservice.cpp \
            qsearchcontroller.cpp \
            qgeocodingservice_nokia.cpp \
            qsearchcontroller_nokia.cpp \
            qgeocodingxmlparser_nokia.cpp \
            qgeocoordinate.cpp \
            qgeolocation.cpp \
            qgeopositioninfo.cpp \
            qgeopositioninfosource.cpp \
            qgeoroute.cpp \
            qgeoroutereply.cpp \
            qgeoroutereply_nokia.cpp \
            qgeorouterequest.cpp \
            qgeoroutexmlparser_nokia.cpp \
            qgeoroutingservice.cpp \
            qgeoroutingservice_nokia.cpp \
            qgeosatelliteinfo.cpp \
            qgeosatelliteinfosource.cpp \
            qlandmark.cpp \
            qlandmarkabstractrequest.cpp \
            qlandmarkboxfilter.cpp \
            qlandmarkcategory.cpp \
            qlandmarkcategoryfetchrequest.cpp \
            qlandmarkcategoryfilter.cpp \
            qlandmarkcategoryid.cpp \
            qlandmarkcategoryidfetchrequest.cpp \
            qlandmarkcategoryremoverequest.cpp \
            qlandmarkcategorysaverequest.cpp \
            qlandmarkcustomfilter.cpp \
            qlandmarkcustomsort.cpp \
            qlandmarkdistancesort.cpp \
            qlandmarkexportrequest.cpp \
            qlandmarkfetchrequest.cpp \
            qlandmarkfilter.cpp \
            qlandmarkid.cpp \
            qlandmarkidfetchrequest.cpp \
            qlandmarkimportrequest.cpp \
            qlandmarkintersectionfilter.cpp \
            qlandmarkmanager.cpp \
            qlandmarkmanagerengine.cpp \
            qlandmarkmanagerenginefactory.cpp \
            qlandmarknamefilter.cpp \
            qlandmarknamesort.cpp \
            qlandmarknearestfilter.cpp \
            qlandmarkproximityfilter.cpp \
            qlandmarkremoverequest.cpp \
            qlandmarksaverequest.cpp \
            qlandmarksortorder.cpp \
            qlandmarkunionfilter.cpp \
            qlocationutils.cpp \
            qmaneuver.cpp \
            qmapellipse.cpp \
            qmapline.cpp \
            qmapmarker.cpp \
            qmapobject.cpp \
            qmappixmap.cpp \
            qmappolygon.cpp \
            qmaprect.cpp \
            qmaproute.cpp \
            qmaptilecache_nokia.cpp \
            qmaptilereply.cpp \
            qmaptilereply_nokia.cpp \
            qmaptileservice.cpp \
            qmaptileservice_nokia.cpp \
            qmapview.cpp \
            qmapview_p.cpp \
            qmapcontainer.cpp \
            qgeoboundingbox.cpp \
            qnmeapositioninfosource.cpp

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
