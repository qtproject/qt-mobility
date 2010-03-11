INCLUDEPATH += $$PWD \
    $${SOURCE_DIR}/src/multimedia/experimental


DEFINES += QMEDIA_GSTREAMER_CAPTURE

HEADERS += $$PWDqgstreamercaptureservice_maemo.h \
    $$PWD\qgstreamercapturesession_maemo.h \
    $$PWD\qgstreameraudioencode_maemo.h \
    $$PWD\qgstreamervideoencode_maemo.h \
    $$PWD\qgstreamerrecordercontrol_maemo.h \
    $$PWD\qgstreamermediacontainercontrol_maemo.h \
    $$PWD\qgstreamercameracontrol_maemo.h \
    $$PWD\qgstreamercapturemetadatacontrol_maemo.h \
    $$PWD\qgstreamerimagecapturecontrol_maemo.h \
    $$PWD\qgstreamerimageencode_maemo.h \
    $$PWD\qgstreamercameraexposurecontrol_maemo.h

SOURCES += $$PWD\qgstreamercaptureservice_maemo.cpp \
    $$PWD\qgstreamercapturesession_maemo.cpp \
    $$PWD\qgstreameraudioencode_maemo.cpp \
    $$PWD\qgstreamervideoencode_maemo.cpp \
    $$PWD\qgstreamerrecordercontrol_maemo.cpp \
    $$PWD\qgstreamermediacontainercontrol_maemo.cpp \
    $$PWD\qgstreamercameracontrol_maemo.cpp \
    $$PWD\qgstreamercapturemetadatacontrol_maemo.cpp \
    $$PWD\qgstreamerimagecapturecontrol_maemo.cpp \
    $$PWD\qgstreamerimageencode_maemo.cpp \
    $$PWD\qgstreamercameraexposurecontrol_maemo.cpp

