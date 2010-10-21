DEFINES += QT_BUILD_BEARER_LIB QT_MAKEDLL

#DEFINES += BEARER_MANAGEMENT_DEBUG

SOURCES += $$PWD/qnetworksession.cpp \
           $$PWD/qnetworkconfigmanager.cpp \
           $$PWD/qnetworkconfiguration.cpp

symbian: {
    contains (occ_enabled, yes) {
        message("Building with OCC enabled")
        DEFINES += OCC_FUNCTIONALITY_AVAILABLE=1
        LIBS += -lextendedconnpref
    } else {
        message("Building without OCC support")
    }
    contains(snap_enabled, yes) {
        message("Building with SNAP support")
        DEFINES += SNAP_FUNCTIONALITY_AVAILABLE=1
        LIBS += -lcmmanager
    } else {
        message("Building without SNAP support")
	LIBS += -lapengine
    }
    
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    HEADERS += $$PWD/qnetworkconfigmanager_s60_p.h \
               $$PWD/qnetworkconfiguration_s60_p.h \
               $$PWD/qnetworksession_s60_p.h
    SOURCES += $$PWD/qnetworkconfigmanager_s60_p.cpp \
               $$PWD/qnetworkconfiguration_s60_p.cpp \
               $$PWD/qnetworksession_s60_p.cpp
               
    LIBS += -lcommdb \
            -lapsettingshandlerui \
            -lconnmon \
            -lcentralrepository \
            -lesock \
            -linsock \
            -lecom \
            -lefsrv \
            -lnetmeta

    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC81
            
    QtBearerManagement.sources = QtBearer.dll
    QtBearerManagement.path = /sys/bin
    DEPLOYMENT += QtBearerManagement
} else {
    maemo6|maemo5 {
        CONFIG += link_pkgconfig
        QT += dbus

        exists(../debug) {
                message("Enabling debug messages.")
                DEFINES += BEARER_MANAGEMENT_DEBUG
        }

        HEADERS += $$PWD/qnetworksession_maemo_p.h \
                   $$PWD/qnetworkconfigmanager_maemo_p.h \
                   $$PWD/qnetworkconfiguration_maemo_p.h

        SOURCES += $$PWD/qnetworkconfigmanager_maemo.cpp \
                   $$PWD/qnetworksession_maemo.cpp

        documentation.path = $$QT_MOBILITY_PREFIX/doc
        documentation.files = doc/html

        PKGCONFIG += glib-2.0 dbus-glib-1 gconf-2.0 osso-ic conninet

        CONFIG += create_pc create_prl
        QMAKE_PKGCONFIG_REQUIRES = glib-2.0 dbus-glib-1 gconf-2.0 osso-ic conninet
        pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
        pkgconfig.files = QtBearer.pc

        INSTALLS += pkgconfig documentation

    } else {

        DEFINES += BEARER_ENGINE

        HEADERS += $$PWD/qnetworkconfigmanager_p.h \
                   $$PWD/qnetworkconfiguration_p.h \
                   $$PWD/qnetworksession_p.h \
                   $$PWD/qnetworksessionengine_p.h \
                   $$PWD/qgenericengine_p.h

        SOURCES += $$PWD/qnetworkconfigmanager_p.cpp \
                   $$PWD/qnetworksession_p.cpp \
                   $$PWD/qnetworksessionengine.cpp \
                   $$PWD/qgenericengine.cpp

        unix:!mac:contains(networkmanager_enabled, yes) {
            contains(QT_CONFIG,dbus) {
                DEFINES += BACKEND_NM
                QT += dbus

                HEADERS += $$PWD/qnmdbushelper_p.h \
                           $$PWD/qnetworkmanagerservice_p.h \
                           $$PWD/qnmwifiengine_unix_p.h

                SOURCES += $$PWD/qnmdbushelper.cpp \
                           $$PWD/qnetworkmanagerservice_p.cpp \
                           $$PWD/qnmwifiengine_unix.cpp
            } else {
                message("NetworkManager backend requires Qt DBus support")
            }
        }

        win32: {
            HEADERS += $$PWD/qnlaengine_win_p.h \
                   $$PWD/qnetworksessionengine_win_p.h

            !wince*:HEADERS += $$PWD/qnativewifiengine_win_p.h

            SOURCES += $$PWD/qnlaengine_win.cpp

            !wince*:SOURCES += $$PWD/qnativewifiengine_win.cpp

            !wince*:LIBS += -lWs2_32
            wince*:LIBS += -lWs2
        }
    }
    macx: {
        HEADERS += $$PWD/qcorewlanengine_mac_p.h
        SOURCES+= $$PWD/qcorewlanengine_mac.mm
        LIBS += -framework Foundation -framework SystemConfiguration

            contains(corewlan_enabled, yes) {
                     isEmpty(QMAKE_MAC_SDK) {
                         SDK6="yes"
                     } else {
                         contains(QMAKE_MAC_SDK, "/Developer/SDKs/MacOSX10.6.sdk") {
                             SDK6="yes"
                     }     
                 }
            
                !isEmpty(SDK6) {
                        LIBS += -framework CoreWLAN -framework Security
                        DEFINES += MAC_SDK_10_6
                }
           }


    }
}

CONFIG += middleware
include($$PWD/../../features/deploy.pri)
