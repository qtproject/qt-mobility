TEMPLATE = subdirs
SUBDIRS += auto testservice2 sampleserviceplugin sampleserviceplugin2
contains(QT_CONFIG,dbus) {
    !symbian:!mac:unix:SUBDIRS += networkmanager
}
symbian:SUBDIRS += bearerex
