TARGET = dialer
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .

HEADERS += dialog.h
SOURCES += dialog.cpp main.cpp
FORMS   += dialog.ui

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
            DEPENDPATH += ./linux
            INCLUDEPATH += ./linux
            include(./linux/dialer.pri)
        }
        maemo* {
            SOURCES += dialer_maemo.cpp
            HEADERS += dialer_maemo.h
        }
    }
}

CONFIG += mobility
mobility += telephony

include(../../examples.pri)
