TEMPLATE = subdirs
CONFIG += ordered

include(../../../common.pri)

## quick platform check.
IS_SYMBIAN_3_PS3 = no
exists($${EPOCROOT}epoc32/data/z/system/install/productid_helen.sis) {
    IS_SYMBIAN_3_PS3 = yes
    message(S^3 1.11 hw79 platform)
}

contains(IS_SYMBIAN_3_PS3, no) {
    exists($${EPOCROOT}epoc32/rom/config/ncp110) {
        IS_SYMBIAN_3_PS3 = yes
        message(S^3 bridge platform)
    }
}

contains(IS_SYMBIAN_3_PS3, no) {
    message(S^3 PS2 or older platform)
}

contains(IS_SYMBIAN_3_PS3, yes) {
    message(Building symbian contacts model library and plugin)
    SUBDIRS += contactsmodel plugin
} else {
    message(Building symbian plugin only)
    SUBDIRS += plugin
}


#contains(symbiancntmodel_enabled, yes) {
#    contains(symbiancntmodelv2_enabled, yes) {
#        ##### Symbian^4 with in-source cntmodel already deployed
#        ##message(Building Symbian CNTMODEL library.)
#        ##SUBDIRS += contactsmodel plugin
#
#        ##TODO FIXME: we disabled building the symbian plugin on Symbian^4 temporarily
#        message(In-source CNTMODEL deployed: building Symbian CNTMODEL and contacts plugin DISABLED!)
#    } else {
#
#        contains(IS_SYMBIAN_4, yes) {
#            ## the configure tests don't agree with the platform check.
#            message(Configure test confusion; building Symbian CNTMODEL and contacts plugin DISABLED!)
#        } else {
#            ## this is symbian 3.1, 3.2, 5.0 or ^3
#            message(Building Symbian contacts plugin)
#            SUBDIRS += plugin
#        }
#    }
#} else {
#    ##### Symbian^4 without in-source cntmodel already deployed
#    ##message(Building Symbian CNTMODEL library.)
#    ##SUBDIRS += contactsmodel plugin
#
#    ##TODO FIXME: we disabled building the symbian plugin on Symbian^4 temporarily
#    message(In-source CNTMODEL not deployed: building Symbian CNTMODEL and contacts plugin DISABLED!)
#}
