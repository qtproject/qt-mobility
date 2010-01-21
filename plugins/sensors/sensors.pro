TEMPLATE = subdirs

include(../../common.pri)

device_plugin=0
maemo5 {
    device_plugin=1
    SUBDIRS += n900
}

SUBDIRS += generic

!equals(device_plugin,1) {
    # Create some dummy sensors to help out with testing
    SUBDIRS += dummy
}

