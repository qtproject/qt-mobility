
INCLUDEPATH += $$PWD

QT += dbus

HEADERS += \
        $$PWD\qgallerytrackeraggregatelistresponse_p.h \
        $$PWD\qgallerytrackerfilelistresponse_p.h \
        $$PWD\qgallerytrackerlistresponse_p.h \
        $$PWD\qgallerytrackerschema_p.h

SOURCES += \
        $$PWD\qdocumentgallery_maemo5.cpp \
        $$PWD\qgallerytrackeraggregatelistresponse.cpp \
        $$PWD\qgallerytrackerfilelistresponse.cpp \
        $$PWD\qgallerytrackerlistresponse.cpp \
        $$PWD\qgallerytrackerschema.cpp
