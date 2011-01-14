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
    maemo6|meego {
        SUBDIRS += \
            qgallerytrackerresultset_maemo6 \
            qgallerytrackerschema_maemo6
    } else {
        SUBDIRS += \
            qgallerytrackerresultset_maemo5 \
            qgallerytrackerschema_maemo5
    }
}

contains(QT_CONFIG, declarative) {
    SUBDIRS += \
        qdeclarativedocumentgalleryitem \
        qdeclarativedocumentgallerymodel \
        qdeclarativedocumentgallerytype
}

