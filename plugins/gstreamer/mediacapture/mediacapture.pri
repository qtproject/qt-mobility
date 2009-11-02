INCLUDEPATH += $$PWD

DEFINES += QMEDIA_GSTREAMER_CAPTURE

HEADERS += $$PWD/qgstreamercaptureservice.h \
    $$PWD/qgstreamercapturesession.h \
    $$PWD/qgstreameraudioencode.h \
    $$PWD/qgstreamervideoencode.h \
    $$PWD/qgstreamerrecordercontrol.h \
    $$PWD/qgstreamermediaformatcontrol.h \
    $$PWD/qgstreamercameracontrol.h \
    $$PWD/qgstreamercapturemetadatacontrol.h \
    $$PWD/qgstreamerimagecapturecontrol.h

SOURCES += $$PWD/qgstreamercaptureservice.cpp \
    $$PWD/qgstreamercapturesession.cpp \
    $$PWD/qgstreameraudioencode.cpp \
    $$PWD/qgstreamervideoencode.cpp \
    $$PWD/qgstreamerrecordercontrol.cpp \
    $$PWD/qgstreamermediaformatcontrol.cpp \
    $$PWD/qgstreamercameracontrol.cpp \
    $$PWD/qgstreamercapturemetadatacontrol.cpp \
    $$PWD/qgstreamerimagecapturecontrol.cpp
