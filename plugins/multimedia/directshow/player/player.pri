INCLUDEPATH += $$PWD

DEFINES += QMEDIA_DIRECTSHOW_PLAYER

HEADERS += \
        $$PWD\directshowglobal.h \
        $$PWD\directshowmetadatacontrol.h \
        $$PWD\directshowplayercontrol.h \
        $$PWD\directshowplayerservice.h \
        $$PWD\directshowrenderthread.h \
        $$PWD\directshowsamplescheduler.h \
        $$PWD\directshowvideooutputcontrol.h \
        $$PWD\directshowvideorenderercontrol.h \
        $$PWD\mediasamplevideobuffer.h \
        $$PWD\videosurfacefilter.h \
        $$PWD\videosurfacemediatype.h \
        $$PWD\videosurfacemediatypeenum.h \
        $$PWD\videosurfacepinenum.h

SOURCES += \
        $$PWD\directshowmetadatacontrol.cpp \
        $$PWD\directshowplayercontrol.cpp \
        $$PWD\directshowplayerservice.cpp \
        $$PWD\directshowrenderthread.cpp \
        $$PWD\directshowsamplescheduler.cpp \
        $$PWD\directshowvideooutputcontrol.cpp \
        $$PWD\directshowvideorenderercontrol.cpp \
        $$PWD\mediasamplevideobuffer.cpp \
        $$PWD\videosurfacefilter.cpp \
        $$PWD\videosurfacemediatype.cpp \
        $$PWD\videosurfacemediatypeenum.cpp \
        $$PWD\videosurfacepinenum.cpp

LIBS += -lstrmiids -ldmoguids -luuid -lmsdmo -lole32 -loleaut32

