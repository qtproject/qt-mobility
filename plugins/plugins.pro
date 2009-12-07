######################################################################
#
# Mobility API project - plugins
#
######################################################################

TEMPLATE = subdirs

contains(mobility_modules,contacts): SUBDIRS += contacts
contains(mobility_modules,multimedia): SUBDIRS += multimedia

