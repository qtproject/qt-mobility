TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
QT+=core

CONFIG += link_pkgconfig qdbus
PKGCONFIG += gypsy gconf-2.0 glib-2.0
