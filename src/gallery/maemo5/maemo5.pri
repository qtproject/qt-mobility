
INCLUDEPATH += $$PWD

QT += dbus

DEFINES += QT_DOCUMENT_GALLERY_MAEMO_5

PRIVATE_HEADERS += \
        $$PWD/qgallerydbusinterface_p.h \
        $$PWD/qgallerytrackerchangenotifier_p.h \
        $$PWD/qgallerytrackereditableresultset_p.h \
        $$PWD/qgallerytrackerlistcolumn_p.h \
        $$PWD/qgallerytrackermetadataedit_p.h \
        $$PWD/qgallerytrackerresultset_p.h \
        $$PWD/qgallerytrackerresultset_p_p.h \
        $$PWD/qgallerytrackerschema_p.h \
        $$PWD/qgallerytrackertyperesultset_p.h

SOURCES += \
        $$PWD/qgallerydbusinterface.cpp \
        $$PWD/qdocumentgallery_maemo5.cpp \
        $$PWD/qgallerytrackerchangenotifier.cpp \
        $$PWD/qgallerytrackereditableresultset.cpp \
        $$PWD/qgallerytrackerlistcolumn.cpp \
        $$PWD/qgallerytrackermetadataedit.cpp \
        $$PWD/qgallerytrackerresultset.cpp \
        $$PWD/qgallerytrackerschema.cpp \
        $$PWD/qgallerytrackertyperesultset.cpp
