#-------------------------------------------------
#
# Project created by QtCreator 2010-06-28T16:36:21
#
#-------------------------------------------------

TARGET = dialer
TEMPLATE = app

SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

win32: {
    SOURCES += dialer_win.cpp
    HEADERS += dialer_win.h
}

symbian: {
    SOURCES += dialer_symbian.cpp
    HEADERS += dialer_symbian.h

} else {
    unix:!mac {
        !maemo* {
            QT += dbus
            DEPENDPATH += /linux
            INCLUDEPATH += /linux
            include(./linux/dialer.pri)
        }
        maemo* {
            SOURCES += dialer_maemo.cpp
            HEADERS += dialer_maemo.h
        }
    }
}

FORMS    += dialog.ui
include(../../examples.pri)
CONFIG += mobility
