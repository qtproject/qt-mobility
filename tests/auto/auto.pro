TEMPLATE = subdirs
#unit tests don't work on symbian yet. enable later
!symbian {
    SUBDIRS += servicemetadata \
           servicedatabase \
           databasemanager \
           qserviceinterfacedescriptor \
           qservicefilter \
           qservicemanager \
           qabstractsecuritysession \
           qservicecontext
}
