# Qt bearer management library
TEMPLATE = lib
TARGET = QtBearer

QT += network

DEFINES += QT_BUILD_BEARER_LIB QT_MAKEDLL

#DEFINES += BEARER_MANAGEMENT_DEBUG

PUBLIC_HEADERS += qnetworkconfiguration.h \
           qnetworksession.h \
           qnetworkconfigmanager.h \
           qbearerglobal.h

HEADERS += $$PUBLIC_HEADERS
SOURCES += qnetworksession.cpp \
           qnetworkconfigmanager.cpp \
           qnetworkconfiguration.cpp

symbian: {
    exists($${EPOCROOT}epoc32/release/winscw/udeb/cmmanager.lib)| \
    exists($${EPOCROOT}epoc32/release/armv5/lib/cmmanager.lib) {
        message("Building with SNAP support")
        DEFINES += SNAP_FUNCTIONALITY_AVAILABLE=1
        LIBS += -lcmmanager
    } else {
        message("Building without SNAP support")
	LIBS += -lapengine
    }
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    HEADERS += qnetworkconfigmanager_s60_p.h \
               qnetworkconfiguration_s60_p.h \
               qnetworksession_s60_p.h
    SOURCES += qnetworkconfigmanager_s60_p.cpp \
               qnetworkconfiguration_s60_p.cpp \
               qnetworksession_s60_p.cpp
               
    LIBS += -lcommdb \
            -lapsettingshandlerui \
            -lconnmon \
            -lcentralrepository \
            -lesock \
            -linsock \
            -lecom \
            -lefsrv \
            -lnetmeta

    deploy.path = $${EPOCROOT}
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    DEPLOYMENT += exportheaders

    TARGET.CAPABILITY = All -TCB
} else {
    maemo {
        QT += dbus
	CONFIG += link_pkgconfig

	exists(../debug) {
		message("Enabling debug messages.")
		DEFINES += BEARER_MANAGEMENT_DEBUG
	}

        HEADERS += qnetworksession_maemo_p.h \
                   qnetworkconfigmanager_maemo_p.h \
                   qnetworkconfiguration_maemo_p.h

        SOURCES += qnetworkconfigmanager_maemo.cpp \
		   qnetworksession_maemo.cpp

	target.path = $$[QT_INSTALL_PREFIX]/lib
	headers.files = $$HEADERS 
	headers.path = $$[QT_INSTALL_PREFIX]/include

	documentation.path = $$[QT_INSTALL_PREFIX]/share/doc/libbearer
 	documentation.files = doc/html

	PKGCONFIG += glib-2.0 dbus-glib-1 gconf-2.0 osso-ic conninet
	DEFINES += MAEMO

	CONFIG += create_pc create_prl
	QMAKE_PKGCONFIG_REQUIRES = glib-2.0 dbus-glib-1 gconf-2.0 osso-ic conninet
	pkgconfig.path = $$[QT_INSTALL_PREFIX]/lib/pkgconfig
	pkgconfig.files = QtBearer.pc

	INSTALLS += pkgconfig target headers documentation

    } else {

        DEFINES += BEARER_ENGINE

        HEADERS += qnetworkconfigmanager_p.h \
                   qnetworkconfiguration_p.h \
                   qnetworksession_p.h \
                   qnetworksessionengine_p.h \
                   qgenericengine_p.h

        SOURCES += qnetworkconfigmanager_p.cpp \
                   qnetworksession_p.cpp \
                   qnetworksessionengine.cpp \
                   qgenericengine.cpp

        unix:!mac:contains(BACKEND, NetworkManager) {
            contains(QT_CONFIG,dbus) {
                DEFINES += BACKEND_NM
                QT += dbus

                HEADERS += qnmdbushelper_p.h \
                           qnetworkmanagerservice_p.h \
                           qnmwifiengine_unix_p.h

                SOURCES += qnmdbushelper.cpp \
                           qnetworkmanagerservice_p.cpp \
                           qnmwifiengine_unix.cpp
            } else {
                message("NetworkManager backend requires Qt DBus support");
            }
        }

        win32: {
            HEADERS += qnlaengine_win_p.h \
                   qnetworksessionengine_win_p.h

            !wince*:HEADERS += qnativewifiengine_win_p.h

            SOURCES += qnlaengine_win.cpp

            !wince*:SOURCES += qnativewifiengine_win.cpp

            !wince*:LIBS += -lWs2_32
            wince*:LIBS += -lWs2
        }
    }
    macx: {
        HEADERS += qcorewlanengine_mac_p.h
        SOURCES+= qcorewlanengine_mac.mm
       # CONFIG-=app_bundle
#CONFIG+=lib_bundle
        LIBS += -framework CoreWLAN
    }
}

QT += network

include (../common.pri)

headers.files = $$PUBLIC_HEADERS
!maemo:headers.path = $$OUTPUT_DIR/include
INSTALLS+=headers
