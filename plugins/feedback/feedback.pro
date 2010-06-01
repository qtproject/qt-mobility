TEMPLATE = subdirs

include(../../common.pri)

#TODO: auto-detect the plugins that need to be built
contains(immersion_enabled, yes) {
    message("Building with Immersion TouchSense support")
    SUBDIRS += immersion
}
