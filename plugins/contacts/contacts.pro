######################################################################
#
# Mobility API project - plugins for contacts
#
######################################################################

TEMPLATE = subdirs
TARGET = 

CONFIG += ordered

include(../../common.pri)

# If qmake has CONTACTS_BACKEND option with kabc included, do that too
contains(CONTACTS_BACKENDS, kabc): SUBDIRS += kabc
contains(CONTACTS_BACKENDS, symbians60): SUBDIRS += symbians60
contains(CONTACTS_BACKENDS, wince): SUBDIRS += wince
