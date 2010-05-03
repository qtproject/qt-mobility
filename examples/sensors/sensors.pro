TEMPLATE = subdirs

include(../../common.pri)

SUBDIRS += accel als arrowkeys compass magneticflux metadata prox rotation tap

contains(QT_CONFIG, declarative) {
    SUBDIRS += orientation
}

SUBDIRS += grueplugin grueapp

SUBDIRS += sensor_explorer

SUBDIRS += reading_perf

SUBDIRS += small_screen_sensors

contains(QT_CONFIG,opengl):SUBDIRS += cubehouse
