INCLUDEPATH += $$PWD

maemo5 {
    QT += dbus
    INCLUDEPATH += /usr/include/gstreamer-0.10;/usr/include/glib-2.0;/usr/lib/glib-2.0/include/;/usr/include/libxml2/
    LIBS += -lgstreamer-0.10 -L/usr/lib -lasound

    HEADERS += \
	$$PWD/v4lradiocontrol_maemo5.h \
	$$PWD/v4lradioservice.h

    SOURCES += \
	$$PWD/v4lradiocontrol_maemo5.cpp \
	$$PWD/v4lradioservice.cpp

} else {

HEADERS += \
    $$PWD/v4lradiocontrol.h \
    $$PWD/v4lradioservice.h

SOURCES += \
    $$PWD/v4lradiocontrol.cpp \
    $$PWD/v4lradioservice.cpp
}