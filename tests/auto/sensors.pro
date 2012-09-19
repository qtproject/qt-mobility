include(../../staticconfig.pri)

requires(contains(mobility_modules,sensors))

TEMPLATE = subdirs
SUBDIRS = qsensor
#SUBDIRS += qsensorgestures
#SUBDIRS += qsensorgestureplugins

#!mac:!win32:SUBDIRS += qsensorgestures_gestures

