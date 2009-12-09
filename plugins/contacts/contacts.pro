######################################################################
#
# Mobility API project - plugins for contacts
#
######################################################################

TEMPLATE = subdirs
TARGET = 

CONFIG += ordered

include(../../common.pri)

# If qmake has CONTACTS_BACKEND option specified, build that backend too
contains(CONTACTS_BACKENDS, symbian): SUBDIRS += symbian
exists($${EPOCROOT}epoc32/data/z/system/install/Series60v5.2.sis) {
	contains(CONTACTS_BACKENDS, symbian): SUBDIRS += symbiansim
}
contains(CONTACTS_BACKENDS, symbian): SUBDIRS += symbian/tsrc
contains(CONTACTS_BACKENDS, wince): SUBDIRS += wince
contains(CONTACTS_BACKENDS, maemo): SUBDIRS += qtcontacts-tracker
