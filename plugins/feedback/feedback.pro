TEMPLATE = subdirs

include(../../common.pri)

contains(immersion_enabled, yes) {
    message("Building with Immersion TouchSense support")
    SUBDIRS += immersion
}

contains(meegotouchfeedback_enabled, yes) {
    message("Building with meegotouch theme haptics support")
    SUBDIRS += meegotouch
}

symbian:SUBDIRS += symbian
# This is MMK specific at the moment - for 1.1.1
equals(QT_MAJOR_VERSION, 4) : greaterThan(QT_MINOR_VERSION, 6):contains(mobility_modules,multimedia):!simulator:SUBDIRS += mmk
simulator:SUBDIRS += simulator
