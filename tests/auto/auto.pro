TEMPLATE = subdirs
SUBDIRS += qnetworkconfigmanager \
           qnetworkconfiguration

win32:SUBDIRS += qnetworksession
maemo:SUBDIRS += qnetworksession
