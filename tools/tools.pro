include(../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,serviceframework) { 
    SUBDIRS += servicedbgen servicexmlgen icheck #SFW API
    !symbian {
    	SUBDIRS += servicefw # not needed for symbian
    }
}

contains(mobility_modules,publishsubscribe) {
    SUBDIRS += vsexplorer qcrmlgen      #P&S API
}
