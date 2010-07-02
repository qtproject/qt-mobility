TEMPLATE = subdirs
CONFIG += ordered

include(../../../common.pri)


## quick platform check.
IS_SYMBIAN_4 = no
exists($${EPOCROOT}epoc32/data/z/system/install/Series60v5.2.sis) {
    exists($${EPOCROOT}epoc32/release/winscw/udeb/VPbkEng.dll) \
    | exists($${EPOCROOT}epoc32/release/armv5/urel/VPbkEng.dll) {
        message(Building Symbian contacts plugin on S^3)
    } else {
        message(Building Symbian contacts plugin on S^4 or later platform)
        IS_SYMBIAN_4 = yes
    }
}


contains(symbiancntmodel_enabled, yes) {
    contains(symbiancntmodelv2_enabled, yes) {
        ##### Symbian^4 with in-source cntmodel already deployed
        ##message(Building Symbian CNTMODEL library.)
        ##SUBDIRS += contactsmodel plugin

        ##TODO FIXME: we disabled building the symbian plugin on Symbian^4 temporarily
        message(In-source CNTMODEL deployed: building Symbian CNTMODEL and contacts plugin DISABLED!)
    } else {

        contains(IS_SYMBIAN_4, yes) {
            ## the configure tests don't agree with the platform check.
            message(Configure test confusion; building Symbian CNTMODEL and contacts plugin DISABLED!)
        } else {
            ## this is symbian 3.1, 3.2, 5.0 or ^3
            message(Building Symbian contacts plugin)
            SUBDIRS += plugin
        }
    }
} else {
    ##### Symbian^4 without in-source cntmodel already deployed
    ##message(Building Symbian CNTMODEL library.)
    ##SUBDIRS += contactsmodel plugin

    ##TODO FIXME: we disabled building the symbian plugin on Symbian^4 temporarily
    message(In-source CNTMODEL not deployed: building Symbian CNTMODEL and contacts plugin DISABLED!)
}
