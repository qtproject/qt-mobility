######################################################################
#
# Mobility API project
#
######################################################################

include($$QT_MOBILITY_BUILD_TREE/config.pri)

TEMPLATE = subdirs
contains(mobility_modules,contacts): SUBDIRS += qtcontactsdocsample
contains(mobility_modules,versit): SUBDIRS += qtversitdocsample
