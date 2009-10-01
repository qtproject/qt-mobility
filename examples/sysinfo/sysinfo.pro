#-------------------------------------------------
#
# Project created by QtCreator 2009-08-06T10:02:21
#
#-------------------------------------------------

QT       += network

TARGET = sysinfo
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

INCLUDEPATH += ../../systeminfo

include(../../common.pri)
LIBS += -lQtSystemInfo

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
