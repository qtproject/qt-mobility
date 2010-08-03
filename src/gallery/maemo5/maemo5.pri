
INCLUDEPATH += $$PWD

QT += dbus

DEFINES += QT_DOCUMENT_GALLERY_MAEMO_5

HEADERS += \
        $$PWD/qgallerydbusinterface_p.h \
        $$PWD/qgallerytrackerchangenotifier_p.h \
        $$PWD/qgallerytrackeritemlist_p.h \
        $$PWD/qgallerytrackeritemlist_p_p.h \
        $$PWD/qgallerytrackeritemresponse_p.h \
        $$PWD/qgallerytrackerlistcolumn_p.h \
        $$PWD/qgallerytrackermetadataedit_p.h \
        $$PWD/qgallerytrackerremoveresponse_p.h \
        $$PWD/qgallerytrackerschema_p.h \
        $$PWD/qgallerytrackertyperesultset_p.h

SOURCES += \
        $$PWD/qgallerydbusinterface.cpp \
        $$PWD/qdocumentgallery_maemo5.cpp \
        $$PWD/qgallerytrackerchangenotifier.cpp \
        $$PWD/qgallerytrackeritemlist.cpp \
        $$PWD/qgallerytrackeritemresponse.cpp \
        $$PWD/qgallerytrackerlistcolumn.cpp \
        $$PWD/qgallerytrackermetadataedit.cpp \
        $$PWD/qgallerytrackerremoveresponse.cpp \
        $$PWD/qgallerytrackerschema.cpp \
        $$PWD/qgallerytrackertyperesultset.cpp
