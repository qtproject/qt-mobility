TEMPLATE = subdirs

include(../../common.pri)

SUBDIRS += accel

contains(QT_CONFIG, declarative) {
    SUBDIRS += orientation
}

