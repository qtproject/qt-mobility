TEMPLATE = subdirs

include(../../common.pri)

device_plugin=0
maemo5 {
    device_plugin=1
    SUBDIRS += n900
}

# Stop the dummy plugin being built on Symbian/Windows CE
symbian:device_plugin=1
wince*:device_plugin=1

SUBDIRS += generic

!equals(device_plugin,1) {
    # Create some dummy sensors to help out with testing
    SUBDIRS += dummy
}

