TEMPLATE = subdirs

include(../../common.pri)

contains(immersion_enabled, yes) {
    message("Building with Immersion TouchSense support")
    SUBDIRS += immersion
}

symbian:SUBDIRS += symbian
contains(QT_CONFIG, phonon):SUBDIRS += phonon
maemo6:SUBDIRS += meegotouch
