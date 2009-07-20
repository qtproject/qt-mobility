TEMPLATE = lib
TARGET = QtSystemInfo
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += qsysteminfo.h qsysteminfo_p.h
SOURCES += qsysteminfo.cpp  
#QT += dbus


win32 {
    SOURCES += qsysteminfo_win.cpp
LIBS += Wlanapi.lib
INCLUDEPATH += "C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include" "C:\Program Files\Microsoft Visual Studio 9.0\VC\include"
}


unix: {
    linux-*: {
    SOURCES += qsysteminfo_linux.cpp
        contains(QT_CONFIG,dbus): {
           QT += dbus
            SOURCES += qhalservice.cpp
            HEADERS += qhalservice.h
       }
    }
    macx: {   }
    symbian: {  }
   
 }

include(../common.pri)
