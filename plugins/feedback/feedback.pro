TEMPLATE = subdirs

include(../../common.pri)

contains(immersion_enabled, yes) {
    message("Building with Immersion TouchSense support")
    SUBDIRS += immersion
}

symbian:SUBDIRS += symbian
contains(mobility_modules,multimedia):SUBDIRS += mmk
maemo6:SUBDIRS += meegotouch
