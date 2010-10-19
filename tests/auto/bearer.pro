include(../../staticconfig.pri)

requires(contains(mobility_modules,bearer))

TEMPLATE = subdirs
SUBDIRS += qnetworkconfigmanager \
    qnetworkconfiguration \
    qnetworksession
