TEMPLATE = subdirs

include(../../common.pri)

SUBDIRS += accel arrowkeys metadata

contains(QT_CONFIG, declarative) {
    SUBDIRS += orientation
}

SUBDIRS += grueplugin grueapp

SUBDIRS += sensor_explorer

SUBDIRS += reading_perf

SUBDIRS += small_screen_sensors

contains(QT_CONFIG,opengl):SUBDIRS += cubehouse

SUBDIRS += show_acceleration show_als show_compass show_magneticflux show_orientation show_proximity show_rotation show_tap
