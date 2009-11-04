TEMPLATE = lib
TARGET = QtSystemInfo
DEPENDPATH += .
INCLUDEPATH += .


QT+= network
# Input
HEADERS += qsysteminfo.h \
    qsysinfoglobal.h
SOURCES += qsysteminfo.cpp
DEFINES += QT_BUILD_SYSINFO_LIB \
    QT_MAKEDLL


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
    #        Strmiids.lib \
            User32.lib \
            Gdi32.lib \
            Ws2_32.lib \
Iphlpapi.lib \
            Oleaut32.lib 
        }

#            Wbemuuid.lib \
#            Wlanapi.lib \
#            Setupapi.lib
#             Bthprops.lib \

    win32-g++ : {
        LIBS += -luser32 -lgdi32

        # -lole32 -lstrmiids -lbthprops -loleaut32 -lsetupapi
        # Wbemuuid Ws2_32
        }

#     DEPENDPATH += C:\Program Files\Microsoft SDKs\Windows\v6.1\Lib

#    INCLUDEPATH += "C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include" \
#        "C:\Program Files\Microsoft Visual Studio 9.0\VC\include" \
#        "C:\Program Files\Microsoft SDKs\Windows\v6.1\Include"

    wince*:LIBS += aygshell.lib \
#        Nleddrv.lib \
        cellcore.lib \
#        Ifapi.lib \
        Coredll.lib
}

unix: {
    QT += gui
    linux-*: {
        SOURCES += qsysteminfo_linux.cpp
        HEADERS += qsysteminfo_linux_p.h
        contains(QT_CONFIG,dbus): {
            QT += dbus
            SOURCES += qhalservice_linux.cpp qnetworkmanagerservice_linux.cpp
            HEADERS += qhalservice_linux_p.h qnetworkmanagerservice_linux_p.h

        }
#        LIBS += -lXxf86vm
    }
    mac: {
        SOURCES += qsysteminfo_mac.mm
        HEADERS += qsysteminfo_mac_p.h
        QMAKE_LFLAGS += -F/Developer/SDKs/MacOSX10.5.sdk/System/Library/Frameworks
        LIBS += -framework IOBluetooth -framework SystemConfiguration -framework CoreFoundation
        INCLUDEPATH += /Developer/SDKs/MacOSX10.5.sdk/System/Library/Frameworks/IOBluetooth.framework/Versions/A/Headers/objc
        TEMPLATE = lib
        #CONFIG += lib_bundle
       macx:debug{
     }

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
            -lcentralrepository \
            -lprofileengine \
            -lbluetooth

        TARGET.CAPABILITY = ALL -TCB
        TARGET.EPOCALLOWDLLDATA = 1
        MMP_RULES += EXPORTUNFROZEN
        

        QtSystemInfoDeployment.sources = QtSystemInfo.dll
        QtSystemInfoDeployment.path = /sys/bin
        DEPLOYMENT += QtSystemInfoDeployment
    }
}
include(../common.pri)
