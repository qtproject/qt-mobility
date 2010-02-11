TEMPLATE = subdirs

include(../../common.pri)

SUBDIRS += accel accel_perf

contains(QT_CONFIG, declarative) {
    SUBDIRS += orientation
}

SUBDIRS += grueplugin grueapp

SUBDIRS += sensor_explorer

SUBDIRS += reading_perf

