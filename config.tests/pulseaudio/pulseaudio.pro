TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp

CONFIG += link_pkgconfig

PKGCONFIG += \
        libpulse \
        libpulse-mainloop-glib
