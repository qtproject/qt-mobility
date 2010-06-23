TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

symbian {
    SUBDIRS += symbian
    contains(build_unit_tests, yes):SUBDIRS += symbian/tsrc
    
    # SIM backend depends on etel MM APIs
    contains(symbiancntsim_enabled, yes) {
        SUBDIRS += symbiansim
        contains(build_unit_tests, yes):SUBDIRS += symbiansim/tsrc
        message("Symbian SIM backend enabled")
    } else {
        message("Symbian SIM backend disabled")
    }
}
wince*:SUBDIRS += wince
maemo5 {
    contains(maemo5-contacts_enabled, yes): SUBDIRS += maemo5
}
