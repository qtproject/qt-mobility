TEMPLATE = lib
TARGET = QtSystemInfo
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += qsysteminfo.h \
    qsysteminfo_p.h \
    qsysinfoglobal.h
SOURCES += qsysteminfo.cpp
DEFINES += QT_BUILD_SYSINFO_LIB \
    QT_MAKEDLL
win32 { 
   QT+= network
    SOURCES += qsysteminfo_win.cpp \
    wmihelper.cpp

    HEADERS += wmihelper.h
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

    INCLUDEPATH += "C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include" \
        "C:\Program Files\Microsoft Visual Studio 9.0\VC\include" \
        "C:\Program Files\Microsoft SDKs\Windows\v6.1\Include"

    wince*:LIBS += aygshell.lib \
        Nleddrv.lib \
        cellcore.lib \
        Ifapi.lib \
        Coredll.lib
}
unix: { 
    linux-*: { 
        QT += gui
        SOURCES += qsysteminfo_linux.cpp
        contains(QT_CONFIG,dbus): { 
            QT += dbus
            SOURCES += qhalservice.cpp
            HEADERS += qhalservice.h
        }
    }
    macx::
    symbian::
}
include(../common.pri)
