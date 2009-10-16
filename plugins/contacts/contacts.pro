TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

# If qmake has CONTACTS_BACKEND option with kabc included, do that too
contains(CONTACTS_BACKENDS, kabc): SUBDIRS += kabc
#disable for now. doesn't compile yet
#contains(CONTACTS_BACKENDS, symbian): SUBDIRS += symbian
contains(CONTACTS_BACKENDS, symbian): SUBDIRS += symbian/tsrc
contains(CONTACTS_BACKENDS, wince): SUBDIRS += wince
