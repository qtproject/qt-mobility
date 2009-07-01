
INCLUDEPATH += $$PWD

DEFINES += QT_NO_VIDEOSURFACE

HEADERS += \
    $$PWD/qaudiodeviceendpoint.h \
    $$PWD/qaudiorendererendpoint.h \
    $$PWD/qiodeviceendpoint.h \
    $$PWD/qmediawidgetendpoint.h \
    $$PWD/qmediawidgetendpoint_p.h \
    $$PWD/qpaintervideosurface_p.h \
    $$PWD/qvideorendererendpoint.h \
    $$PWD/qvideorendererwidget_p.h

SOURCES += \
    $$PWD/qaudiodeviceendpoint.cpp \
    $$PWD/qaudiorendererendpoint.cpp \
    $$PWD/qiodeviceendpoint.cpp \
    $$PWD/qmediawidgetendpoint.cpp \
    $$PWD/qpaintervideosurface.cpp \
    $$PWD/qvideorendererendpoint.cpp \
    $$PWD/qvideorendererwidget.cpp
