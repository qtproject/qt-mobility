INCLUDEPATH += $$PWD

DEFINES += QMEDIA_GSTREAMER_CAPTURE

HEADERS += $$PWD/qgstreamercaptureservice.h \
    $$PWD/maemo/qgstreamercapturesession_maemo.h \
    $$PWD/maemo/qgstreameraudioencode_maemo.h \
    $$PWD/maemo/qgstreamervideoencode_maemo.h \
    $$PWD/maemo/qgstreamerrecordercontrol_maemo.h \
    $$PWD/maemo/qgstreamermediacontainercontrol_maemo.h \
    $$PWD/qgstreamercameracontrol.h \
    $$PWD/qgstreamercapturemetadatacontrol.h \
    $$PWD/qgstreamerimagecapturecontrol.h \
    $$PWD/maemo/qgstreamerimageencode_maemo.h

SOURCES += $$PWD/qgstreamercaptureservice.cpp \
    $$PWD/maemo/qgstreamercapturesession_maemo.cpp \
    $$PWD/maemo/qgstreameraudioencode_maemo.cpp \
    $$PWD/maemo/qgstreamervideoencode_maemo.cpp \
    $$PWD/maemo/qgstreamerrecordercontrol_maemo.cpp \
    $$PWD/maemo/qgstreamermediacontainercontrol_maemo.cpp \
    $$PWD/qgstreamercameracontrol.cpp \
    $$PWD/qgstreamercapturemetadatacontrol.cpp \
    $$PWD/qgstreamerimagecapturecontrol.cpp \
    $$PWD/maemo/qgstreamerimageencode_maemo.cpp
