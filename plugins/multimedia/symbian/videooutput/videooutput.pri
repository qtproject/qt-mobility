INCLUDEPATH += $$PWD

message("VideoOutput: using common implementation")

include(../trace/trace.pri)

HEADERS += $$PWD/s60videodisplay.h           \
           $$PWD/s60videooutpututils.h       \
           $$PWD/s60videooutputfactory.h     \
           $$PWD/s60videowidget.h            \
           $$PWD/s60videowidgetcontrol.h     \
           $$PWD/s60videowidgetdisplay.h     \
           $$PWD/s60videowindowcontrol.h     \
           $$PWD/s60videowindowdisplay.h

SOURCES += $$PWD/s60videodisplay.cpp         \
           $$PWD/s60videooutpututils.cpp     \
           $$PWD/s60videooutputfactory.cpp   \
           $$PWD/s60videowidget.cpp          \
           $$PWD/s60videowidgetcontrol.cpp   \
           $$PWD/s60videowidgetdisplay.cpp   \
           $$PWD/s60videowindowcontrol.cpp   \
           $$PWD/s60videowindowdisplay.cpp

LIBS *= -lcone
LIBS *= -lws32

# Uncomment this to enable frame rate measurement
#videooutput_measure_framerate = yes

contains(surfaces_s60_enabled, yes) {
    message("VideoOutput: graphics surface rendering supported")
    DEFINES += VIDEOOUTPUT_GRAPHICS_SURFACES
    HEADERS += $$PWD/s60eglendpoint.h               \
               $$PWD/s60eglextensions.h             \
               $$PWD/s60nativewindow.h              \
               $$PWD/s60videobuffer.h               \
               $$PWD/s60videoeglrenderercontrol.h
    SOURCES += $$PWD/s60eglendpoint.cpp             \
               $$PWD/s60eglextensions.cpp           \
               $$PWD/s60nativewindow.cpp            \
               $$PWD/s60videobuffer.cpp             \
               $$PWD/s60videoeglrenderercontrol.cpp
    DEFINES += VIDEOOUTPUT_EGL_RENDERER
    LIBS += -llibegl
    contains(QT_CONFIG, opengl) | contains(QT_CONFIG, opengles2) {
        QT += opengl
    } else {
        DEFINES += QT_NO_OPENGL
    }
    contains(QT_CONFIG, openvg) {
        LIBS += -llibopenvg
    } else {
        DEFINES += QT_NO_OPENVG
    }
    contains(videooutput_measure_framerate, yes) {
        HEADERS += $$PWD/s60videoframerate.h
        SOURCES += $$PWD/s60videoframerate.cpp
        DEFINES += VIDEOOUTPUT_MEASURE_FRAMERATE
    }
} else {
    message("VideoOutput: no graphics surface rendering support - DSA only")
}

exists($$[QT_INSTALL_HEADERS]/QtGui/private/qwidget_p.h) {
    DEFINES += PRIVATE_QTGUI_HEADERS_AVAILABLE
    message("VideoOutput: private QtGui headers are available")
} else {
    message("VideoOutput: private QtGui headers not available - video and viewfinder may not be rendered correctly")
}
