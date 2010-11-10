TEMPLATE = lib

TARGET = QtGallery

include (../../common.pri)
INCLUDEPATH+= .

!static:DEFINES += QT_MAKEDLL
DEFINES += QT_BUILD_GALLERY_LIB

PRIVATE_HEADERS = \
        qabstractgallery_p.h \
        qgalleryabstractrequest_p.h \
        qgalleryabstractresponse_p.h \
        qgallerynullresultset_p.h \
        qgalleryresultset_p.h

PUBLIC_HEADERS = \
        qabstractgallery.h \
        qdocumentgallery.h \
        qgalleryabstractrequest.h \
        qgalleryabstractresponse.h \
        qgalleryfilter.h \
        qgalleryitemrequest.h \
        qgalleryproperty.h \
        qgalleryquerymodel.h \
        qgalleryqueryrequest.h \
        qgalleryresource.h \
        qgalleryresultset.h \
        qgallerytype.h \
        qgallerytyperequest.h

SOURCES = \
        qabstractgallery.cpp \
        qdocumentgallery.cpp \
        qgalleryabstractrequest.cpp \
        qgalleryabstractresponse.cpp \
        qgalleryfilter.cpp \
        qgalleryitemrequest.cpp \
        qgalleryquerymodel.cpp \
        qgalleryqueryrequest.cpp \
        qgalleryresource.cpp \
        qgalleryresultset.cpp \
        qgallerytyperequest.cpp

OTHER_FILES = \
        qgalleryproperty.qdoc \
        qgallerytype.qdoc

simulator: {
    include (simulator/simulator.pri)
} else:unix:contains(QT_CONFIG, dbus) {
    maemo5:include (maemo5/maemo5.pri)
    maemo6:include (maemo6/maemo6.pri)
} else:symbian:contains(mds_enabled, yes) {
     include (symbian/symbian.pri)
} else {
    DEFINES += QT_DOCUMENT_GALLERY_NULL
}

HEADERS = $$PUBLIC_HEADERS $$PRIVATE_HEADERS

symbian {
    load(data_caging_paths)
    QtGalleryDeployment.sources = QtGallery.dll
    QtGalleryDeployment.path = /sys/bin
    DEPLOYMENT += QtGalleryDeployment
    TARGET.UID3=0x2002BFCB
    TARGET.CAPABILITY = ALL -TCB
}

CONFIG += middleware
include(../../features/deploy.pri)
