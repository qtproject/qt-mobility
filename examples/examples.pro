TEMPLATE = subdirs
SUBDIRS = publish-subscribe

contains(QT_CONFIG, declarative) {
    SUBDIRS += battery-charge
}
