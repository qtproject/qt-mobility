
INCLUDEPATH += $$PWD

DEFINES += QT_NO_VIDEOSURFACE

HEADERS += \
    $$PWD/qpaintervideosurface_p.h \
    $$PWD/qvideorenderermediaoutput.h \
    $$PWD/qvideorendererwidget_p.h \
    $$PWD/qwidgetmediaoutput.h \
    $$PWD/qwidgetmediaoutput_p.h

SOURCES += \
    $$PWD/qpaintervideosurface.cpp \
    $$PWD/qvideorenderermediaoutput.cpp \
    $$PWD/qvideorendererwidget.cpp \
    $$PWD/qwidgetmediaoutput.cpp
