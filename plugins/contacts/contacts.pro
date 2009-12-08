TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

symbian:SUBDIRS += symbian #symbiansim
wince*:SUBDIRS += wince
maemo:SUBDIRS += qtcontacts-tracker
