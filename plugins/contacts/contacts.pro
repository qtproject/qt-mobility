TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

# If qmake has CONTACTS_BACKEND option specified, build that backend too
contains(CONTACTS_BACKENDS, symbian): SUBDIRS += symbian
contains(CONTACTS_BACKENDS, wince): SUBDIRS += wince
contains(CONTACTS_BACKENDS, maemo): SUBDIRS += qtcontacts-tracker
