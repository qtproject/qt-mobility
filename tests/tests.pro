TEMPLATE = subdirs
SUBDIRS += auto testservice2 sampleserviceplugin sampleserviceplugin2
contains(QT_CONFIG,dbus) {
    !symbian:!mac:!maemo:unix:SUBDIRS += networkmanager
}
symbian:SUBDIRS += bearerex
