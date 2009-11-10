TEMPLATE = lib
TARGET = QtSystemInfo


QT+= network
include(../../common.pri)

# Input
PUBLIC_HEADERS +=   qsysteminfo.h \
                    qsysinfoglobal.h
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
            Ole32.lib \
            User32.lib \
            Gdi32.lib \
            Ws2_32.lib \
            Iphlpapi.lib \
            Oleaut32.lib 
    }

    win32-g++ : {
        LIBS += -luser32 -lgdi32
    }


    wince*:LIBS += aygshell.lib \
        cellcore.lib \
        Coredll.lib
}

unix: {
    QT += gui
    linux-*: {
        SOURCES += qsysteminfo_linux.cpp
        HEADERS += qsysteminfo_linux_p.h
        contains(networkmanager_enabled, yes):contains(QT_CONFIG,dbus): {
            QT += dbus
            SOURCES += qhalservice_linux.cpp qnetworkmanagerservice_linux.cpp
            HEADERS += qhalservice_linux_p.h qnetworkmanagerservice_linux_p.h

        }
    }

    mac: {
        SOURCES += qsysteminfo_mac.mm
        HEADERS += qsysteminfo_mac_p.h
        LIBS += -framework SystemConfiguration -framework CoreFoundation \
         -framework IOKit -framework ApplicationServices -framework Foundation \
         -framework CoreServices

        # change this to /Developer/SDKs/MacOSX10.6.sdk
        # if you want to compile for 10.6 with CoreWLAN framework

        QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.5.sdk
        contains(QMAKE_MAC_SDK, "/Developer/SDKs/MacOSX10.6.sdk") {
                LIBS += -framework CoreWLAN  -framework IOBluetooth
                DEFINES += MAC_SDK_10_6
        }
        TEMPLATE = lib
    }

    symbian:{
        INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
        DEPENDPATH += symbian
        
        SOURCES += qsysteminfo_s60.cpp \
            telephonyinfo_s60.cpp

        HEADERS += qsysteminfo_s60_p.h \
            telephonyinfo_s60.h

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
            -lws32 \
            -lcentralrepository

        TARGET.CAPABILITY = ALL -TCB
        TARGET.EPOCALLOWDLLDATA = 1
        MMP_RULES += EXPORTUNFROZEN
        

        QtSystemInfoDeployment.sources = QtSystemInfo.dll
        QtSystemInfoDeployment.path = /sys/bin
        DEPLOYMENT += QtSystemInfoDeployment
    }
}

HEADERS += $$PUBLIC_HEADERS 
include (../../features/deploy.pri)
