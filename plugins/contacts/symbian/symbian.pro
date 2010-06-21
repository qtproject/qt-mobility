TEMPLATE = subdirs

contains(build_symbiancntmodel, yes) {
    message(Building Symbian CNTMODEL library.)
    SUBDIRS += contactsmodel 
}
SUBDIRS += plugin

CONFIG += ordered
