include(../staticconfig.pri)

TEMPLATE = subdirs
SUBDIRS += global

simulator:SUBDIRS += mobilitysimulator

contains(mobility_modules,serviceframework) {
    SUBDIRS += serviceframework
    symbian {
        SUBDIRS += serviceframework/databasemanagerserver_symbian/databasemanagerserver.pro
    }
}

contains(mobility_modules,bearer){
    SUBDIRS += bearer
    symbian {
        !contains(MOBILITY_SD_MCL_BUILD, yes):exists($${EPOCROOT}epoc32/release/winscw/udeb/z/system/install/series60v5.2.sis)|exists($${EPOCROOT}epoc32/data/z/system/install/series60v5.2.sis)|exists($${EPOCROOT}epoc32/release/armv5/lib/libstdcppv5.dso): SUBDIRS += bearer/bearer_old
    }
}
    
contains(mobility_modules,location): SUBDIRS += location
contains(mobility_modules,multimedia): SUBDIRS += multimedia

## The order must be contacts, organizer, versit, versitorganizer
## so that qmake creates the .prl files correctly.
contains(mobility_modules,contacts): SUBDIRS += contacts
contains(mobility_modules,organizer): SUBDIRS += organizer
contains(mobility_modules,versit) {
    SUBDIRS += versit
    contains(mobility_modules,organizer): SUBDIRS += versitorganizer
}

contains(mobility_modules,feedback): SUBDIRS += feedback

contains(mobility_modules,publishsubscribe) {
    SUBDIRS += publishsubscribe
    symbian {
        SUBDIRS += publishsubscribe/psmapperserver_symbian/pspathmapperserver.pro
    }
}

contains(mobility_modules,systeminfo): SUBDIRS += systeminfo
contains(mobility_modules,sensors): SUBDIRS += sensors
contains(mobility_modules,connectivity): SUBDIRS += connectivity

# Versit depends on Contacts
versit.subdir=versit
versit.target=sub-versit
versit.depends=contacts

# VersitOrganizer depends on Versit and Organizer
versitorganizer.subdir=versitorganizer
versitorganizer.target=sub-versitorganizer
versitorganizer.depends=versit organizer

simulator|contains(qmf_enabled, yes)|wince*|win32|symbian|maemo5|maemo6 {
    contains(mobility_modules,messaging) {
        simulator|!win32-g++:SUBDIRS += messaging
    }
}

contains(mobility_modules,gallery): SUBDIRS += gallery

contains(mobility_modules,landmarks): SUBDIRS += landmarks

landmarks.subdir=landmarks
landmarks.target=sub-landmarks
landmarks.depends=location

# Connectivity (nfc)  depends on Service Framework
connectivity.subdir=connectivity
connectivity.target=sub-connectivity
connectivity.depends=serviceframework

symbian {
    SUBDIRS += s60installs
}

