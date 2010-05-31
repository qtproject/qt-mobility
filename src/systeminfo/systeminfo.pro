TEMPLATE = lib
TARGET = QtSystemInfo


QT+= network
include(../../common.pri)

# Input
PUBLIC_HEADERS +=   qsysteminfo.h
SOURCES += qsysteminfo.cpp
DEFINES += QT_BUILD_SYSINFO_LIB QT_MAKEDLL


win32 {
    contains(CONFIG,release) {
       CONFIG-=console
    }
    SOURCES += qsysteminfo_win.cpp 
    HEADERS += qsysteminfo_win_p.h 

    win32-msvc*: {
        SOURCES += qwmihelper_win.cpp
        HEADERS += qwmihelper_win_p.h

        LIBS += \
            -lOle32 \
            -lUser32 \
            -lGdi32 \
            -lIphlpapi \
            -lOleaut32
    }

    win32-g++ : {
        LIBS += -luser32 -lgdi32
    }


    wince*:LIBS += -Laygshell \
        -lcellcore \
        -lCoredll
}

unix: {
    QT += gui
    maemo5|maemo6|linux-*: {
        SOURCES += qsysteminfo_linux_common.cpp
        HEADERS += qsysteminfo_linux_common_p.h
    }
    !maemo5:!maemo6:linux-*: {
        SOURCES += qsysteminfo_linux.cpp
        HEADERS += qsysteminfo_linux_p.h
        contains(QT_CONFIG,dbus): {
            QT += dbus
            SOURCES += qhalservice_linux.cpp
            HEADERS += qhalservice_linux_p.h
                contains(networkmanager_enabled, yes): {
                    SOURCES += qnetworkmanagerservice_linux.cpp qnmdbushelper.cpp
                    HEADERS += qnetworkmanagerservice_linux_p.h qnmdbushelper_p.h
                } else {
                DEFINES += QT_NO_NETWORKMANAGER
                }
        } else {
           DEFINES += QT_NO_NETWORKMANAGER
        }
    }
    maemo5|maemo6: {
            #Qt GConf wrapper added here until a proper place is found for it.
            CONFIG += link_pkgconfig
            SOURCES += qsysteminfo_maemo.cpp gconfitem.cpp
            HEADERS += qsysteminfo_maemo_p.h gconfitem_p.h
        contains(QT_CONFIG,dbus): {
                QT += dbus
                SOURCES += qhalservice_linux.cpp
                HEADERS += qhalservice_linux_p.h
       }
       PKGCONFIG += glib-2.0 gconf-2.0
       CONFIG += create_pc create_prl
       QMAKE_PKGCONFIG_REQUIRES = glib-2.0 gconf-2.0
       pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
       pkgconfig.files = QtSystemInfo.pc
    }

    mac: {
        SOURCES += qsysteminfo_mac.mm
        HEADERS += qsysteminfo_mac_p.h
        LIBS += -framework SystemConfiguration -framework CoreFoundation \
         -framework IOKit -framework ApplicationServices -framework Foundation \
         -framework CoreServices -framework ScreenSaver -framework QTKit \
         -framework IOBluetooth

            contains(corewlan_enabled, yes) {
                     isEmpty(QMAKE_MAC_SDK) {
                         SDK6="yes"
                     } else {
                         contains(QMAKE_MAC_SDK, "/Developer/SDKs/MacOSX10.6.sdk") {
                             SDK6="yes"
                     }     
                 }
            
                !isEmpty(SDK6) {
                        LIBS += -framework CoreWLAN  -framework CoreLocation
                        DEFINES += MAC_SDK_10_6
                }
           } else {
               CONFIG += no_keywords
           }

    TEMPLATE = lib
    }

    symbian:{
        contains(S60_VERSION, 3.1){
            DEFINES += SYMBIAN_3_1
        }        
        INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
        INCLUDEPATH += symbian/stubs
        DEPENDPATH += symbian
        
        SOURCES += qsysteminfo_s60.cpp \
            telephonyinfo_s60.cpp \
            chargingstatus_s60.cpp

        HEADERS += qsysteminfo_s60_p.h \
            telephonyinfo_s60.h \
            chargingstatus_s60.h

        LIBS += -lprofileengine \
            -letel3rdparty \
            -lsysutil \
            -lcentralrepository \
            -lcenrepnotifhandler \
            -lefsrv \
            -lptiengine \
            -lfeatdiscovery \
            -lhwrmvibraclient \
            -lavkon \    #Used by AknLayoutUtils::PenEnabled(). Try to remove this dependency.
            -lcone \
            -lws32 \
            -lcentralrepository \
            -lprofileengine \
            -lbluetooth \
            -lgdi

        TARGET.CAPABILITY = ALL -TCB
#        TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData UserEnvironment Location ReadDeviceData TrustedUI

        TARGET.EPOCALLOWDLLDATA = 1
        TARGET.UID3 = 0x2002ac7d

        QtSystemInfoDeployment.sources = QtSystemInfo.dll
        QtSystemInfoDeployment.path = /sys/bin
        DEPLOYMENT += QtSystemInfoDeployment
    }
}

HEADERS += $$PUBLIC_HEADERS 
CONFIG += middleware
include (../../features/deploy.pri)
