TEMPLATE = subdirs

include(../../common.pri)

maemo5 {
    SUBDIRS += n900
}

maemo6 {
    equals(sensord_enabled,yes) {
        SUBDIRS += maemo6
    } else {
        message("sensord library missing")
    }
}

symbian {
    equals(sensors_symbian_enabled,yes) {
        SUBDIRS += symbian
    } else:equals(sensors_s60_31_enabled,yes) {
        SUBDIRS += s60_sensor_api
    } else {
        message("No native sensor interface found")
    }
}

SUBDIRS += generic
simulator: SUBDIRS += simulator

# Allow forcing extra plugins to build even if they won't run
BUILD_ALL_PLUGINS=$$(BUILD_ALL_PLUGINS)
equals(BUILD_ALL_PLUGINS,1) {
    SUBDIRS *= n900 dummy
}

