TEMPLATE = subdirs

include(../../staticconfig.pri)

contains(mobility_modules,connectivity):    SUBDIRS+=connectivity.pro

