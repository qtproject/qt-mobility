TEMPLATE = subdirs

DEPENDPATH += $$PWD/../multimedia

SUBDIRS = mpd

contains(QT_CONFIG, phonon): SUBDIRS += phonon

win32: SUBDIRS += wmp
mac: SUBDIRS += qt7
unix:contains(QT_CONFIG, gstreamer): SUBDIRS += gstreamer

