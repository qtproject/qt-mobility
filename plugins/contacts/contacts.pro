TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

symbian {
    SUBDIRS += symbian
    #contains(build_unit_tests, yes):SUBDIRS += symbian/tsrc
    
    # SIM backend depends on etel MM APIs
    exists($${EPOCROOT}epoc32/release/winscw/udeb/etelmm.lib) \
    | exists($${EPOCROOT}epoc32/release/armv5/lib/etelmm.lib) {
        SUBDIRS += symbiansim
        contains(build_unit_tests, yes):SUBDIRS += symbiansim/tsrc
        message("SIM backend enabled")
    } else {
        message("SIM backend disabled")
    }
}
wince*:SUBDIRS += wince
maemo6:SUBDIRS += qtcontacts-tracker
