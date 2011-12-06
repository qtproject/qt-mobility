include(../../features/utils.pri)

DEFINES += QT_BUILD_BEARER_LIB QT_MAKEDLL

#DEFINES += BEARER_MANAGEMENT_DEBUG

SOURCES += qnetworksession.cpp \
           qnetworkconfigmanager.cpp \
           qnetworkconfiguration.cpp

HEADERS += qnetworksession.h \
           qnetworkconfiguration.h \
           qnetworkconfigmanager.h \
           qnetworksession_p.h \
           qnetworksessionadaptor_p.h \
           qnetworkconfigmanager_p.h \
           qnetworkconfigmanageradaptor_p.h

symbian: {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = $$mobilityUID(0x2002AC81)
    QtBearerManagement.sources = QtBearer$${QT_LIBINFIX}.dll
    QtBearerManagement.path = /sys/bin
    QtBearerManagement.pkg_prerules += $${LITERAL_HASH}{\"QtBearer\"},(0x2002AC81),1,2,1,TYPE=SA,RU
    DEPLOYMENT += QtBearerManagement
} else {
    maemo6|maemo5 {
        CONFIG += link_pkgconfig
        QT += dbus

        exists(../debug) {
                message("Enabling debug messages.")
                DEFINES += BEARER_MANAGEMENT_DEBUG
        }

        documentation.path = $$QT_MOBILITY_PREFIX/doc
        documentation.files = doc/html

        PKGCONFIG += glib-2.0 dbus-glib-1 gconf-2.0 osso-ic conninet

        QMAKE_PKGCONFIG_REQUIRES = glib-2.0 dbus-glib-1 gconf-2.0 osso-ic conninet

        INSTALLS += documentation

    }
}

CONFIG += middleware
include(../../features/deploy.pri)
