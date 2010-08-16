include(../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,serviceframework) { 
    SUBDIRS += servicedbgen servicefw servicexmlgen icheck  #SFW API
}

contains(mobility_modules,publishsubscribe) {
    SUBDIRS += vsexplorer qcrmlgen      #P&S API
}
