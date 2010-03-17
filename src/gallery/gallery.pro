TEMPLATE = lib

TARGET = QtGallery

include (../../common.pri)
INCLUDEPATH+= .

!static:DEFINES += QT_MAKEDLL
DEFINES += QT_BUILD_GALLERY_LIB

PRIVATE_HEADERS = \
        qabstractgallery_p.h \
        qgalleryerrorresponse_p.h

PUBLIC_HEADERS = \
        qgallery.h \
        qgalleryfilter.h \
        qgalleryitemlist.h \
        qgalleryrequest.h

SOURCES = \
        qgallery.cpp \
        qgalleryerrorresponse.cpp \
        qgalleryfilter.cpp \
        qgalleryitemlist.cpp \
        qgalleryrequest.cpp

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

win32 {
    include (ws4/ws4.pri)
} else {
    DEFINES += QT_DOCUMENT_GALLERY_NULL
}

symbian {
    load(data_caging_paths)
    QtMediaDeployment.sources = QtGallery.dll
    QtMediaDeployment.path = /sys/bin
    DEPLOYMENT += QtGalleryDeployment
    TARGET.UID3=0x2002AC77
    MMP_RULES += EXPORTUNFROZEN
    TARGET.CAPABILITY = ALL -TCB
}

CONFIG += middleware
include(../../features/deploy.pri)
