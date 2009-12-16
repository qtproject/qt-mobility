TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

symbian:SUBDIRS += symbian
wince*:SUBDIRS += wince
maemo:SUBDIRS += qtcontacts-tracker
