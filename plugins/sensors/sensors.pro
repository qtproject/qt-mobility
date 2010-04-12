TEMPLATE = subdirs

include(../../common.pri)

device_plugin=0
maemo5 {
    device_plugin=1
    SUBDIRS += n900
}

symbian {
    equals(sensors_symbian_enabled,yes) {
        device_plugin=1                                                    
        SUBDIRS += symbian
    } else:equals(sensors_s60_31_enabled,yes) {
        device_plugin=1
        SUBDIRS += s60_sensor_api
    } else {
        message("No native sensor interface found")
    }
}

SUBDIRS += generic

!equals(device_plugin,1) {
    # Create some dummy sensors to help out with testing
    SUBDIRS += dummy
}

BUILD_ALL_PLUGINS=$$(BUILD_ALL_PLUGINS)
equals(BUILD_ALL_PLUGINS,1) {
    SUBDIRS *= n900 dummy
}

