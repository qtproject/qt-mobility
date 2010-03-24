include(../../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,serviceframework): SUBDIRS += sampleserviceplugin  serviceframework
contains(mobility_modules,contacts): SUBDIRS += contacts
contains(mobility_modules,systeminfo): SUBDIRS += qsysteminfo
#contains(mobility_modules,messaging) {
#    contains(qmf_enabled,yes)|wince*|win32|symbian|maemo5 {
#        !win32-g++: SUBDIRS += messaging
#    }
#}

