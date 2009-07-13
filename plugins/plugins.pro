TEMPLATE = subdirs

SUBDIRS *= mpd

win32: SUBDIRS *= wmp
mac: SUBDIRS += qt7

