include(../../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,serviceframework): SUBDIRS += sampleserviceplugin  serviceframework
contains(mobility_modules,contacts): SUBDIRS += contacts
contains(mobility_modules,systeminfo): SUBDIRS += qsysteminfo
contains(mobility_modules,messaging): SUBDIRS += messaging

