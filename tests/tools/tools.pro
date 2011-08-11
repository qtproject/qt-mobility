include(../../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,serviceframework): SUBDIRS += testservice2 sampleserviceplugin sampleserviceplugin2

contains(mobility_modules,connectivity) {

    SUBDIRS += nfctestserver

    linux*:!linux-armcc:contains(bluez_enabled, yes):contains(QT_CONFIG, dbus) {
        SUBDIRS += btclient
    }
}

