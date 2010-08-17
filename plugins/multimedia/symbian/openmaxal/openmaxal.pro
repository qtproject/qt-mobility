# pro file for OpenMAX AL

TEMPLATE = subdirs

CONFIG += ordered

# needed for the contains check
include (../../../../common.pri)

# Input parameters for the generated bld.inf file
# -----------------------------------------------
SYMBIAN_PLATFORMS = DEFAULT

contains(openmaxal_symbian_enabled, yes) {
    message("Building OpenMAX AL backend.")
    SUBDIRS = mediarecorder
}