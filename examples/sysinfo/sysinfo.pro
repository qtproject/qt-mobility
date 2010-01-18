QT       += network

TARGET = sysinfo
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

INCLUDEPATH += ../../src/systeminfo

include(../examples.pri)
CONFIG += mobility
MOBILITY = systeminfo

CONFIG += console

win32 {
    FORMS += dialog.ui
}

unix: {
    linux-*: {
        FORMS += dialog.ui
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
