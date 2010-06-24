include(../staticconfig.pri)

TEMPLATE = subdirs
SUBDIRS += global

contains(mobility_modules,serviceframework) {
    SUBDIRS += serviceframework
    symbian {
        SUBDIRS += serviceframework/databasemanagerserver_symbian/databasemanagerserver.pro
    }
}

contains(mobility_modules,bearer): SUBDIRS += bearer
contains(mobility_modules,location): SUBDIRS += location
contains(mobility_modules,multimedia): SUBDIRS += multimedia
contains(mobility_modules,contacts): SUBDIRS += contacts

contains(mobility_modules,publishsubscribe) {
    SUBDIRS += publishsubscribe
    symbian {
        SUBDIRS += publishsubscribe/psmapperserver_symbian/pspathmapperserver.pro
    }
}

contains(mobility_modules,systeminfo): SUBDIRS += systeminfo
contains(mobility_modules,versit): SUBDIRS += versit
contains(mobility_modules,sensors): SUBDIRS += sensors
contains(mobility_modules,connectivity): SUBDIRS += connectivity

# Versit depends on Contacts
versit.subdir=versit
versit.target=sub-versit
versit.depends=contacts

contains(qmf_enabled, yes)|wince*|win32|symbian|maemo5 {
    contains(mobility_modules,messaging) {
        !win32-g++:SUBDIRS += messaging
    }
}

symbian {
    SUBDIRS += s60installs
}

