######################################################################
#
# Mobility API project - multimedia 
#
######################################################################

TEMPLATE = subdirs

SUBDIRS += m3u

win32 {
    contains(QT_CONFIG, multimedia): SUBDIRS += audiocapture
}

win32:!wince* {
    win32-msvc2005|win32-msvc2008: SUBDIRS *= directshow wmp
}

unix:!mac:!symbian {
    TMP_GST_LIBS = \
        gstreamer-0.10 >= 0.10.19 \
        gstreamer-base-0.10 >= 0.10.19 \
        gstreamer-interfaces-0.10 >= 0.10.19 \
        gstreamer-audio-0.10 >= 0.10.19 \
        gstreamer-video-0.10 >= 0.10.19

    system(pkg-config --exists \'$${TMP_GST_LIBS}\' --print-errors): {
        SUBDIRS += gstreamer
    } else {
        contains(QT_CONFIG, multimedia): SUBDIRS += audiocapture
    }
   
    SUBDIRS += v4l
}

mac {
    contains(QT_CONFIG, multimedia): SUBDIRS += audiocapture
    SUBDIRS += qt7
}

symbian:SUBDIRS += symbian
