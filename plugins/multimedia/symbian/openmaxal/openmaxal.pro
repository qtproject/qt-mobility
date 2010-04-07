TEMPLATE = subdirs

CONFIG += ordered

# Input parameters for the generated bld.inf file
# -----------------------------------------------
SYMBIAN_PLATFORMS = DEFAULT

# Check to see if the SDK supports OpenMAX AL API
exists($${EPOCROOT}epoc32/include/platform/mw/khronos/OpenMAXAL.h) {
    SUBDIRS = mediarecorder
    message("Building OpenMAX AL backend.")
}
