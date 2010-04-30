TEMPLATE = lib
TARGET = maemo6sensorplugin
DEPENDPATH += .
INCLUDEPATH += .
PLUGIN_TYPE = sensors
QT += dbus
QT += network
CONFIG += plugin
CONFIG += sensord
CONFIG += link_pkgconfig
CONFIG += mobility
HEADERS += maemo6sensorbase.h \
    maemo6accelerometer.h \
    maemo6als.h \
    maemo6compass.h \
    maemo6magnetometer.h \
    maemo6orientationsensor.h \
    maemo6proximitysensor.h \
    maemo6rotationsensor.h \
    maemo6tapsensor.h
SOURCES += maemo6sensorbase.cpp \
    maemo6accelerometer.cpp \
    maemo6als.cpp \
    maemo6compass.cpp \
    maemo6magnetometer.cpp \
    maemo6orientationsensor.cpp \
    maemo6proximitysensor.cpp \
    maemo6rotationsensor.cpp \
    maemo6tapsensor.cpp \
    main.cpp
target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS += target
