include(../../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,sensors): SUBDIRS += sensors
contains(mobility_modules,serviceframework): SUBDIRS += serviceframework
contains(mobility_modules,telephony): SUBDIRS += telephony
contains(mobility_modules,publishsubscribe): SUBDIRS += publishsubscribe
contains(mobility_modules,multimedia): SUBDIRS += multimedia
contains(mobility_modules,systeminfo): SUBDIRS += systeminfo
contains(mobility_modules,contacts):contains(mobility_modules,versit) SUBDIRS += contacts
