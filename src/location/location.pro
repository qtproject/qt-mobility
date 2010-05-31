TEMPLATE = lib
TARGET = QtLocation
QT = core gui network sql

include(../../common.pri)

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

INCLUDEPATH += .
DEPENDPATH += .

PUBLIC_HEADERS += \
                    qgeoaddress.h \
                    qgeoareamonitor.h \
                    qgeoboundingbox.h \
                    qgeocoordinate.h \
                    qgeodistance.h \
                    qgeonavigationinstruction.h \
                    #qgeonavigator.h \
                    qgeomappingmanager.h \
                    qgeomapreply.h \
                    qgeomapviewport.h \
                    qgeomapwidget.h \
                    qgeoplace.h \
                    qgeoplacesmanager.h \
                    qgeoplacesreply.h \
                    qgeopositioninfo.h \
                    qgeopositioninfosource.h \
                    qgeoroute.h \
                    qgeoroutereply.h \
                    qgeorouterequest.h \
                    qgeoroutesegment.h \
                    qgeoroutingmanager.h \
                    qgeosatelliteinfo.h \
                    qgeosatelliteinfosource.h \
                    qgeoserviceprovider.h \
                    qgeoserviceproviderplugin.h \
                    qgeotiledmappingmanager.h \
                    qgeotiledmapviewport.h \
                    qlandmark.h \
                    qlandmarkabstractrequest.h \
                    qlandmarkattributefilter.h \
                    qlandmarkboxfilter.h \
                    qlandmarkcategory.h \
                    qlandmarkcategoryfetchrequest.h \
                    qlandmarkcategoryfilter.h \
                    qlandmarkcategoryid.h \
                    qlandmarkcategoryidfetchrequest.h \
                    qlandmarkcategoryremoverequest.h \
                    qlandmarkcategorysaverequest.h \
                    qlandmarkdistancesort.h \
                    qlandmarkexportrequest.h \
                    qlandmarkfetchrequest.h \
                    qlandmarkfilter.h \
                    qlandmarkid.h \
                    qlandmarkidfetchrequest.h \
                    qlandmarkidfilter.h \
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
                    qnmeapositioninfosource.h 

PRIVATE_HEADERS += \
                    qgeoaddress_p.h \
                    qgeoboundingbox_p.h \
                    qgeodistance_p.h \
                    qgeomappingmanager_p.h \
                    qgeomapreply_p.h \
                    qgeomapviewport_p.h \
                    qgeomapwidget_p.h \
                    qgeonavigationinstruction_p.h \
                    #qgeonavigator_p.h \
                    qgeoplace_p.h \
                    qgeoplacesmanager_p.h \
                    qgeoplacesreply_p.h \
                    qgeoroute_p.h \
                    qgeoroutereply_p.h \
                    qgeorouterequest_p.h \
                    qgeoroutesegment_p.h \
                    qgeoroutingmanager_p.h \
                    qgeoserviceprovider_p.h \
                    qgeotiledmappingmanager_p.h \
                    qgeotiledmapviewport_p.h \
                    qlandmark_p.h \
                    qlandmarkabstractrequest_p.h \
                    qlandmarkboxfilter_p.h \
                    qlandmarkcategory_p.h \
                    qlandmarkcategoryfilter_p.h \
                    qlandmarkcategoryid_p.h \
                    qlandmarkdistancesort_p.h \
                    qlandmarkfilehandler_gpx_p.h \
                    qlandmarkfilehandler_lmx_p.h \
                    qlandmarkfilter_p.h \
                    qlandmarkid_p.h \
                    qlandmarkidfilter_p.h \
                    qlandmarkintersectionfilter_p.h \
                    qlandmarkmanager_p.h \
                    qlandmarknamefilter_p.h \
                    qlandmarknamesort_p.h \
                    qlandmarknearestfilter_p.h \
                    qlandmarkproximityfilter_p.h \
                    qlandmarkrequests_p.h \
                    qlandmarksortorder_p.h \
                    qlandmarkunionfilter_p.h \
                    qlocationutils_p.h \
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
    HEADERS += gconfitem_p.h  \
               liblocationwrapper_p.h \
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
            qgeoboundingbox.cpp \
            qgeocoordinate.cpp \
            qgeodistance.cpp \
            qgeonavigationinstruction.cpp \
            #qgeonavigator.cpp \
            qgeomappingmanager.cpp \
            qgeomapreply.cpp \
            qgeomapviewport.cpp \
            qgeomapwidget.cpp \
            qgeoplace.cpp \
            qgeoplacesmanager.cpp \
            qgeoplacesreply.cpp \
            qgeopositioninfo.cpp \
            qgeopositioninfosource.cpp \
            qgeoroute.cpp \
            qgeoroutereply.cpp \
            qgeorouterequest.cpp \
            qgeoroutesegment.cpp \
            qgeoroutingmanager.cpp \
            qgeosatelliteinfo.cpp \
            qgeosatelliteinfosource.cpp \
            qgeoserviceprovider.cpp \
            qgeoserviceproviderplugin.cpp \
            qgeotiledmappingmanager.cpp \
            qgeotiledmapviewport.cpp \
            qlandmark.cpp \
            qlandmarkabstractrequest.cpp \
            qlandmarkattributefilter.cpp \
            qlandmarkboxfilter.cpp \
            qlandmarkcategory.cpp \
            qlandmarkcategoryfetchrequest.cpp \
            qlandmarkcategoryfilter.cpp \
            qlandmarkcategoryid.cpp \
            qlandmarkcategoryidfetchrequest.cpp \
            qlandmarkcategoryremoverequest.cpp \
            qlandmarkcategorysaverequest.cpp \
            qlandmarkdistancesort.cpp \
            qlandmarkexportrequest.cpp \
            qlandmarkfetchrequest.cpp \
            qlandmarkfilehandler_gpx.cpp \
            qlandmarkfilehandler_lmx.cpp \
            qlandmarkfilter.cpp \
            qlandmarkid.cpp \
            qlandmarkidfetchrequest.cpp \
            qlandmarkidfilter.cpp \
            qlandmarkimportrequest.cpp \
            qlandmarkintersectionfilter.cpp \
            qlandmarkmanager.cpp \
            qlandmarkmanager_p.cpp \
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
            qnmeapositioninfosource.cpp

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC83
    MMP_RULES += EXPORTUNFROZEN

    INCLUDEPATH += $${EPOCROOT}epoc32\include\osextensions \
                   $${EPOCROOT}epoc32\include\LBTHeaders \
                   $${EPOCROOT}epoc32\include\platform
    LIBS += -llbs
    LIBS += -lefsrv
    contains(lbt_enabled, yes) {
        LIBS += -llbt
    }

    QtLocationDeployment.sources = QtLocation.dll
    QtLocationDeployment.path = /sys/bin
    DEPLOYMENT += QtLocationDeployment
}

CONFIG += middleware
include(../../features/deploy.pri)
