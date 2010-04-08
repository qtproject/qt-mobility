
INCLUDEPATH += $$PWD

QT += dbus

HEADERS += \
        $$PWD\qgallerytrackeraggregatelistresponse_p.h \
        $$PWD\qgallerytrackercountresponse_p.h \
        $$PWD\qgallerytrackerfilecopyresponse_p.h \
        $$PWD\qgallerytrackerfileeditresponse_p.h \
        $$PWD\qgallerytrackerfilelistresponse_p.h \
        $$PWD\qgallerytrackerfilemoveresponse_p.h \
        $$PWD\qgallerytrackerfileremoveresponse_p.h \
        $$PWD\qgallerytrackerlistresponse_p.h \
        $$PWD\qgallerytrackerschema_p.h

SOURCES += \
        $$PWD\qdocumentgallery_maemo5.cpp \
        $$PWD\qgallerytrackeraggregatelistresponse.cpp \
        $$PWD\qgallerytrackercountresponse.cpp \
        $$PWD\qgallerytrackerfilecopyresponse.cpp \
        $$PWD\qgallerytrackerfileeditresponse.cpp \
        $$PWD\qgallerytrackerfilelistresponse.cpp \
        $$PWD\qgallerytrackerlistresponse.cpp \
        $$PWD\qgallerytrackerfilemoveresponse.cpp \
        $$PWD\qgallerytrackerfileremoveresponse.cpp \
        $$PWD\qgallerytrackerschema.cpp
