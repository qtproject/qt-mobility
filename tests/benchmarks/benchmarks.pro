include($$QT_MOBILITY_BUILD_TREE/config.pri)

TEMPLATE = subdirs

contains(mobility_modules,serviceframework): SUBDIRS += sampleserviceplugin  serviceframework
contains(mobility_modules,contacts): SUBDIRS += contacts
contains(mobility_modules,systeminfo): SUBDIRS += qsysteminfo

