TEMPLATE = subdirs

include(../../common.pri)

maemo5 {
    SUBDIRS += n900
}

symbian {
    equals(sensors_symbian_enabled,yes) {
    } else:equals(sensors_s60_31_enabled,yes) {
        SUBDIRS += s60_sensor_api
    } else {
        message("No native sensor interface found")
    }
}

SUBDIRS += generic

BUILD_ALL_PLUGINS=$$(BUILD_ALL_PLUGINS)
equals(BUILD_ALL_PLUGINS,1) {
    SUBDIRS *= n900
}

