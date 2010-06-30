TEMPLATE = subdirs
CONFIG += ordered

include(../../../common.pri)

contains(symbiancntmodel_enabled, yes) {
    SUBDIRS += plugin
} else {
    ##message(Building Symbian CNTMODEL library.)
    ##SUBDIRS += contactsmodel plugin

    ##TODO FIXME: we disabled building the symbian plugin on Symbian^4 temporarily
    message(Building Symbian CNTMODEL and contacts plugin DISABLED!)
}
