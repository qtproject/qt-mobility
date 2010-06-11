include(../../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,serviceframework): SUBDIRS += serviceframework
contains(mobility_modules,telephony): SUBDIRS += telephony
