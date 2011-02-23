TEMPLATE = subdirs
CONFIG += ordered

include(../../../common.pri)

## quick platform check.
IS_SYMBIAN_3_PS3 = no
exists($${EPOCROOT}epoc32/data/z/system/install/Symbianv4.0.sis) {
    IS_SYMBIAN_3_PS3 = yes
    message(Building Symbian contacts plugin on S^3 PS3 or later platform)
} else {
    message(Building Symbian contacts plugin on S^3 PS2 or older platform)     
}

contains(IS_SYMBIAN_3_PS3, yes) {
    message(Building Symbian^3 PS3 CNTMODEL library and plugin)
    SUBDIRS += contactsmodel plugin
} else {
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
