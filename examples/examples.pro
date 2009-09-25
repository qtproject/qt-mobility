TEMPLATE = subdirs
SUBDIRS = publish-subscribe contextkit

contains(QT_CONFIG, declarative) {
    SUBDIRS += battery-charge
}
