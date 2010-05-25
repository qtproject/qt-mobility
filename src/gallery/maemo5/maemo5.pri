
INCLUDEPATH += $$PWD

QT += dbus

HEADERS += \
        $$PWD/qgallerydbusinterface_p.h \
        $$PWD/qgallerytrackeraggregateresponse_p.h \
        $$PWD/qgallerytrackercountresponse_p.h \
        $$PWD/qgallerytrackeritemlist_p.h \
        $$PWD/qgallerytrackeritemlist_p_p.h \
        $$PWD/qgallerytrackeritemresponse_p.h \
        $$PWD/qgallerytrackerlistcolumn_p.h \
        $$PWD/qgallerytrackermetadataedit_p.h \
        $$PWD/qgallerytrackerremoveresponse_p.h \
        $$PWD/qgallerytrackerschema_p.h \
        $$PWD/qgallerytrackerthumbnailcolumn_p.h \
        $$PWD/qgallerytrackerurlresponse_p.h

SOURCES += \
        $$PWD/qdocumentgallery_maemo5.cpp \
        $$PWD/qgallerytrackeraggregateresponse.cpp \
        $$PWD/qgallerytrackercountresponse.cpp \
        $$PWD/qgallerytrackeritemlist.cpp \
        $$PWD/qgallerytrackeritemresponse.cpp \
        $$PWD/qgallerytrackerlistcolumn.cpp \
        $$PWD/qgallerytrackermetadataedit.cpp \
        $$PWD/qgallerytrackerremoveresponse.cpp \
        $$PWD/qgallerytrackerschema.cpp \
        $$PWD/qgallerytrackerthumbnailcolumn.cpp \
        $$PWD/qgallerytrackerurlresponse.cpp
