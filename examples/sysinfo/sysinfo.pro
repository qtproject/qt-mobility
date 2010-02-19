QT       += network

TARGET = sysinfo
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h
RESOURCES = examples.qrc

INCLUDEPATH += ../../src/systeminfo

include(../examples.pri)
CONFIG += mobility
MOBILITY = systeminfo

CONFIG += console

win32 {
    FORMS += dialog.ui
}

unix: {
    !maemo*:linux-*: {
        FORMS += dialog.ui
    } maemo* {
    FORMS    += dialog_s60.ui
   }
    
    mac: {
        FORMS += dialog.ui
    }
}

symbian {
    TARGET.CAPABILITY = LocalServices NetworkServices ReadUserData WriteUserData UserEnvironment Location  ReadDeviceData TrustedUI
    TARGET.UID3 = 0x2002ac7e
    FORMS    += dialog_s60.ui
}
