include(../../staticconfig.pri)

requires(contains(mobility_modules,sensors))

TEMPLATE = subdirs
SUBDIRS = qsensor
