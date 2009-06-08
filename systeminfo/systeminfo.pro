TEMPLATE = lib
TARGET = QtSystemInformation
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += qsystemsinfo.h qsystemsinfo_p.h
SOURCES += qsystemsinfo.cpp  

include(../common.pri)

win32 {
    SOURCES += qsystemsinfo_win.cpp
LIBS += Wlanapi.lib
INCLUDEPATH += "C:\Program Files\Microsoft SDKs\Windows\v6.0A\Include" "C:\Program Files\Microsoft Visual Studio 9.0\VC\include"
}


unix: {
    linux-*: { SOURCES += qsystemsinfo_linux.cpp }
    macx: {   }
    symbian: {  }
   
 }
