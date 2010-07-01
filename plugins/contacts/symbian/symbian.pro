TEMPLATE = subdirs
CONFIG += ordered

include(../../../common.pri)

contains(symbiancntmodel_enabled, yes) {
    contains(symbiancntmodelv2_enabled, yes) {
        ##### Symbian^4 with in-source cntmodel already deployed
        ##message(Building Symbian CNTMODEL library.)
        ##SUBDIRS += contactsmodel plugin

        ##TODO FIXME: we disabled building the symbian plugin on Symbian^4 temporarily
        message(Building Symbian CNTMODEL and contacts plugin DISABLED!)
    } else {
        ## this is symbian 3.1, 3.2, 5.0 or ^3
        message(Building Symbian contacts plugin)
        SUBDIRS += plugin
    }
} else {
    ##### Symbian^4 without in-source cntmodel already deployed
    ##message(Building Symbian CNTMODEL library.)
    ##SUBDIRS += contactsmodel plugin

    ##TODO FIXME: we disabled building the symbian plugin on Symbian^4 temporarily
    message(Building Symbian CNTMODEL and contacts plugin DISABLED!)
}
