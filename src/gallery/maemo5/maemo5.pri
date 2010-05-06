
INCLUDEPATH += $$PWD

QT += dbus

HEADERS += \
        $$PWD/qgallerydbusinterface_p.h \
        $$PWD/qgalleryimageloader_p.h \
        $$PWD/qgallerymediaartloader_p.h \
        $$PWD/qgallerythumbnailloader_p.h \
        $$PWD/qgallerytrackeraggregatelistresponse_p.h \
        $$PWD/qgallerytrackercountresponse_p.h \
        $$PWD/qgallerytrackerfileeditresponse_p.h \
        $$PWD/qgallerytrackerfileremoveresponse_p.h \
        $$PWD/qgallerytrackeritemlist_p.h \
        $$PWD/qgallerytrackeritemlistresponse_p.h \
        $$PWD/qgallerytrackerlistcolumn_p.h \
        $$PWD/qgallerytrackerlistresponse_p.h \
        $$PWD/qgallerytrackermetadataedit_p.h \
        $$PWD/qgallerytrackerschema_p.h \
        $$PWD/qgallerytrackerurlresponse_p.h

SOURCES += \
        $$PWD/qdocumentgallery_maemo5.cpp \
        $$PWD/qgalleryimageloader.cpp \
        $$PWD/qgallerymediaartloader.cpp \
        $$PWD/qgallerythumbnailloader.cpp \
        $$PWD/qgallerytrackeraggregatelistresponse.cpp \
        $$PWD/qgallerytrackercountresponse.cpp \
        $$PWD/qgallerytrackerfileeditresponse.cpp \
        $$PWD/qgallerytrackerfileremoveresponse.cpp \
        $$PWD/qgallerytrackeritemlist.cpp \
        $$PWD/qgallerytrackeritemlistresponse.cpp \
        $$PWD/qgallerytrackerlistcolumn.cpp \
        $$PWD/qgallerytrackerlistresponse.cpp \
        $$PWD/qgallerytrackermetadataedit.cpp \
        $$PWD/qgallerytrackerschema.cpp \
        $$PWD/qgallerytrackerurlresponse.cpp
