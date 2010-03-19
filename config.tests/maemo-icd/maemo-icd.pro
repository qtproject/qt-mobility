TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
QT+=dbus

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0 dbus-glib-1 gconf-2.0 osso-ic conninet
