TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

symbian:SUBDIRS += symbian
wince*:SUBDIRS += wince
maemo6:SUBDIRS += qtcontacts-tracker
