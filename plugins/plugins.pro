TEMPLATE = subdirs

SUBDIRS *= mpd phonon

win32: SUBDIRS *= wmp
mac: SUBDIRS += qt7
unix:!mac: SUBDIRS += gstreamer

