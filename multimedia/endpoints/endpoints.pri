
INCLUDEPATH += $$PWD

# DEFINES += AUDIOSERVICES

multimedia {
    QT += multimedia
} else {
    DEFINES += QT_NO_VIDEOSURFACE
}

HEADERS += \
    $$PWD/qaudiodeviceendpoint.h \
    $$PWD/qaudiorendererendpoint.h \
    $$PWD/qiodeviceendpoint.h \
    $$PWD/qmediaendpointinterface.h \
    $$PWD/qmediawidgetendpoint.h \
    $$PWD/qmediawidgetendpoint_p.h \
    $$PWD/qpaintervideosurface_p.h \
    $$PWD/qvideooverlayendpoint.h \
    $$PWD/qvideorendererendpoint.h \
    $$PWD/qvideorendererwidget_p.h

SOURCES += \
    $$PWD/qaudiodeviceendpoint.cpp \
    $$PWD/qaudiorendererendpoint.cpp \
    $$PWD/qiodeviceendpoint.cpp \
    $$PWD/qmediaendpointinterface.cpp \
    $$PWD/qmediawidgetendpoint.cpp \
    $$PWD/qpaintervideosurface.cpp \
    $$PWD/qvideooverlayendpoint.cpp \
    $$PWD/qvideorendererendpoint.cpp \
    $$PWD/qvideorendererwidget.cpp
