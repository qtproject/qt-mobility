TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
QT+=core

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0 libebook-1.2 libosso-abook-1.0 
