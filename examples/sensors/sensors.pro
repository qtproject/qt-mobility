TEMPLATE = subdirs

include(../../common.pri)

SUBDIRS += accel accel_perf

contains(QT_CONFIG, declarative) {
    SUBDIRS += orientation
}

