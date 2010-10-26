include(../../staticconfig.pri)

requires(contains(mobility_modules,serviceframework))

TEMPLATE = subdirs
SUBDIRS += servicemetadata \
    qserviceinterfacedescriptor \
    qservicefilter \
    qservicemanager \
    qabstractsecuritysession \
    qservicecontext \
    qmetaobjectbuilder \
    icheck \
    qservicemanager_ipc \
    servicedatabase
    # databasemanager # disabled from auto builds

