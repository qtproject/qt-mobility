######################################################################
#
# Mobility API project - Symbian backends
#
######################################################################

TEMPLATE = subdirs

include (../../../config.pri)
symbian:SUBDIRS += mmf 

contains(openmaxal_symbian_enabled, yes) {
    message("Enabling OpenMAX AL audio record backend")
    symbian:SUBDIRS += openmaxal
}
