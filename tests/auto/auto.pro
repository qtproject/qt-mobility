TEMPLATE = subdirs
SUBDIRS += qnetworkconfigmanager \
           qnetworkconfiguration

win32:SUBDIRS += qnetworksession
unix:SUBDIRS += qnetworksession
