TEMPLATE = subdirs
SUBDIRS += servicemetadata \                #service framework
           servicedatabase \
           databasemanager \
           qserviceinterfacedescriptor \
           qservicefilter \
           qservicemanager \
           qabstractsecuritysession \
           qservicecontext \
           qnetworkconfigmanager \          #Bearer management
           qnetworkconfiguration

win32:SUBDIRS += qnetworksession
