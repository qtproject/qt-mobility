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
        qgalleryresultset_p.h

PUBLIC_HEADERS = \
        qabstractgallery.h \
        qdocumentgallery.h \
        qgalleryabstractrequest.h \
        qgalleryabstractresponse.h \
        qgalleryfilter.h \
        qgalleryproperty.h \
        qgalleryquerymodel.h \
        qgalleryqueryrequest.h \
        qgalleryremoverequest.h \
        qgalleryresource.h \
        qgalleryresultset.h \
        qgallerytype.h

SOURCES = \
        qabstractgallery.cpp \
        qdocumentgallery.cpp \
        qgalleryabstractrequest.cpp \
        qgalleryabstractresponse.cpp \
        qgalleryfilter.cpp \
        qgalleryproperty.cpp \
        qgalleryquerymodel.cpp \
        qgalleryqueryrequest.cpp \
        qgalleryremoverequest.cpp \
        qgalleryresource.cpp \
        qgalleryresultset.cpp \
        qgallerytype.cpp

HEADERS = $$PUBLIC_HEADERS $$PRIVATE_HEADERS

unix: contains(QT_CONFIG, dbus): include (maemo5/maemo5.pri)
else: DEFINES += QT_DOCUMENT_GALLERY_NULL

symbian {
    load(data_caging_paths)
    QtGalleryDeployment.sources = QtGallery.dll
    QtGalleryDeployment.path = /sys/bin
    DEPLOYMENT += QtGalleryDeployment
    TARGET.UID3=0x2002BFCB
    MMP_RULES += EXPORTUNFROZEN
    TARGET.CAPABILITY = ALL -TCB
}

CONFIG += middleware
include(../../features/deploy.pri)
