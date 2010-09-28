include(../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,serviceframework) { 
    SUBDIRS += servicedbgen servicefw servicexmlgen #SFW API
    !symbian {
    	SUBDIRS += icheck # broken headers on tb10.1, temp disable
    }
}

contains(mobility_modules,publishsubscribe) {
    SUBDIRS += vsexplorer qcrmlgen      #P&S API
}
