TEMPLATE = subdirs

include(../../common.pri)

contains(immersion_enabled, yes) {
    message("Building with Immersion TouchSense support")
    SUBDIRS += immersion
}

symbian:SUBDIRS += symbian
# This is MMK specific at the moment.
equals(QT_MAJOR_VERSION, 4) : greaterThan(QT_MINOR_VERSION, 6):contains(mobility_modules,multimedia):SUBDIRS += mmk
maemo6:SUBDIRS += meegotouch
