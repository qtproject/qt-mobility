TEMPLATE = subdirs

DEPENDPATH += $$PWD/../multimedia

SUBDIRS = mpd

contains(QT_CONFIG, phonon): SUBDIRS += phonon


win32 {
    TMP_INCLUDE = $$quote($$(INCLUDE))
    TMP_SEARCHPATHS = $$split(TMP_INCLUDE, ";") $$QMAKE_INCDIR
    for(p, TMP_SEARCHPATHS): exists($${p}/wmp.h): SUBDIRS *= wmp
}

mac: SUBDIRS += qt7
unix:contains(QT_CONFIG, gstreamer): SUBDIRS += gstreamer

