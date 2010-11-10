include(../../staticconfig.pri)

requires(contains(mobility_modules,gallery))

TEMPLATE = subdirs

SUBDIRS += \
    qdocumentgallery \
    qgalleryabstractrequest \
    qgalleryabstractresponse \
    qgalleryfilter \
    qgalleryitemrequest \
    qgalleryquerymodel \
    qgalleryqueryrequest \
    qgalleryresource \
    qgallerytyperequest

unix: contains(QT_CONFIG, dbus): {
    maemo5: {
        SUBDIRS += \
            qgallerytrackerresultset_maemo5 \
            qgallerytrackerschema_maemo5
    }
    maemo6: {
        SUBDIRS += \
            qgallerytrackerschema_maemo6
    }
}

contains(QT_CONFIG, declarative) {
    SUBDIRS += \
        qdeclarativedocumentgalleryitem \
        qdeclarativedocumentgallerymodel \
        qdeclarativedocumentgallerytype
}

