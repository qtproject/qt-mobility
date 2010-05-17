TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

#wince*:SUBDIRS += wince
symbian:SUBDIRS += symbian
maemo6:SUBDIRS += maemo6
maemo5:SUBDIRS += maemo5
