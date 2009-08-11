TEMPLATE = subdirs

SUBDIRS = mpd

contains(QT_CONFIG, phonon): SUBDIRS += phonon

win32 {
    TMP_INCLUDE = $$quote($$(INCLUDE))
    TMP_SEARCHPATHS = $$split(TMP_INCLUDE, ";") $$QMAKE_INCDIR
    for(p, TMP_SEARCHPATHS): exists($${p}/wmp.h): SUBDIRS *= wmp
}

mac: SUBDIRS += qt7
unix: system(pkg-config --exists 'gstreamer-audio-0.10 gstreamer-video-0.10'): SUBDIRS += gstreamer

