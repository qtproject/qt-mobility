QT       += network

TARGET = sysinfo
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

INCLUDEPATH += ../../systeminfo

include(../examples.pri)
qtAddLibrary(QtSystemInfo)

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
    TARGET.CAPABILITY = ALL -TCB
    FORMS    += dialog_s60.ui
}
