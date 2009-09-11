TEMPLATE = lib
TARGET = QtSystemInfo

QT+= network
include(../common.pri)

# Input
PUBLIC_HEADERS += qsysteminfo.h \
    qsysinfoglobal.h
SOURCES += qsysteminfo.cpp
DEFINES += QT_BUILD_SYSINFO_LIB QT_MAKEDLL

win32 { 
    SOURCES += qsysteminfo_win.cpp \
    qwmihelper_win.cpp
    HEADERS += qsysteminfo_win_p.h \
        qwmihelper_win_p.h

    LIBS += Wlanapi.lib \
        Ole32.lib \
        Strmiids.lib \
        Bthprops.lib \
        User32.lib \
        Gdi32.lib \
        Ws2_32.lib \
        Wbemuuid.lib \
        Oleaut32.lib \
        Setupapi.lib

     #DEPENDPATH += C:\Program Files\Microsoft SDKs\Windows\v6.1\Lib

#    INCLUDEPATH += "C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include" \
        #"C:\Program Files\Microsoft Visual Studio 9.0\VC\include" \
 #       "C:\Program Files\Microsoft SDKs\Windows\v6.1\Include"

    wince*:LIBS += aygshell.lib \
        Nleddrv.lib \
        cellcore.lib \
        Ifapi.lib \
        Coredll.lib
}
unix: {
    QT += gui
    linux-*: {
        SOURCES += qsysteminfo_linux.cpp
        HEADERS += qsysteminfo_linux_p.h
        contains(QT_CONFIG,dbus): {
            QT += dbus
            SOURCES += qhalservice_linux.cpp
            HEADERS += qhalservice_linux_p.h

        }
        #LIBS += -lXxf86vm
    }
    mac: {
        SOURCES += qsysteminfo_mac.cpp
        HEADERS += qsysteminfo_mac_p.h
    }
    symbian::
}

HEADERS += $$PUBLIC_HEADERS 
include (../features/deploy.pri)
