include(../../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,multimedia): SUBDIRS += multimedia
