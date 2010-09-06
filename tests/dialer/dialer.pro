TARGET = dialer
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .

HEADERS += dialog.h
SOURCES += dialog.cpp main.cpp
FORMS   += dialog.ui

include(../../common.pri)

linux-*:!maemo* {
    QT += dbus
    DEPENDPATH += ./linux
    INCLUDEPATH += ./linux
    include(./linux/dialer.pri)
}
else {
    SOURCES += dialer_unsupported.cpp
    HEADERS += dialer_unsupported.h
}

CONFIG += mobility
mobility += telephony
