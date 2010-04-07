TEMPLATE = subdirs

include(../../common.pri)

device_plugin=0
maemo5 {
    device_plugin=1
    SUBDIRS += n900
}

symbian {
    exists($${EPOCROOT}epoc32/release/armv5/lib/SensrvClient.lib) {
        message("Sensor framework found")
        device_plugin=1  
        SUBDIRS += symbian
    } else {
        exists($${EPOCROOT}epoc32/release/armv5/lib/RRSensorApi.lib) {
            message("Sensor API found")
            device_plugin=1
            SUBDIRS += s60_sensor_api
        } else {
            message("No native sensor interface found")
            device_plugin=0
        }
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

