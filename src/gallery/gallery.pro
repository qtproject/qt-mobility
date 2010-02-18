TEMPLATE = lib

TARGET = QtGallery

include (../../common.pri)
INCLUDEPATH+= .

!static:DEFINES += QT_MAKEDLL
DEFINES += QT_BUILD_GALLERY_LIB

HEADERS = \
        qgallery.h \
        qgallerydocumentlist.h \
        qgalleryfilter.h \
        qgalleryrequest.h

SOURCES = \
        qgallery.cpp \
        qgallerydocumentlist.cpp \
        qgalleryfilter.cpp \
        qgalleryrequest.cpp

symbian {
    load(data_caging_paths)
    QtMediaDeployment.sources = QtGallery.dll
    QtMediaDeployment.path = /sys/bin
    DEPLOYMENT += QtGalleryDeployment
    TARGET.UID3=0x2002AC77
    MMP_RULES += EXPORTUNFROZEN
    TARGET.CAPABILITY = ALL -TCB
}

