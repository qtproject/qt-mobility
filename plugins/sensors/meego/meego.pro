INCLUDEPATH+=../../../src/sensors

PLUGIN_TYPE = sensors

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_meego)

include(version.pri)
include(meego.pri)
include(../../../common.pri)

QT = core dbus network
CONFIG += mobility
MOBILITY += sensors
CONFIG += link_pkgconfig
PKGCONFIG += sensord

CONFIGFILES.files = Sensors.conf
CONFIGFILES.path = /etc/xdg/Nokia/
INSTALLS += CONFIGFILES
