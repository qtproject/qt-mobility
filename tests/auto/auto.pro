TEMPLATE = subdirs
SUBDIRS += qnetworkconfigmanager \
           qnetworkconfiguration

win32:SUBDIRS += qnetworksession
!mac:unix:SUBDIRS += qnetworksession
