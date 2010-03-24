TEMPLATE = subdirs

include(../../common.pri)

SUBDIRS += accel

contains(QT_CONFIG, declarative) {
    SUBDIRS += orientation
}

SUBDIRS += grueplugin grueapp

SUBDIRS += sensor_explorer

SUBDIRS += reading_perf

