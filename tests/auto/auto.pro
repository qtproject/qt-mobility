TEMPLATE = subdirs
SUBDIRS += qnetworkconfigmanager \
           qnetworkconfiguration

!mac:unix:win32:SUBDIRS += qnetworksession

