include($$QT_MOBILITY_BUILD_TREE/config.pri)

TEMPLATE = subdirs
SUBDIRS += global

contains(mobility_modules,serviceframework) {
    SUBDIRS += serviceframework
    symbian {
        SUBDIRS = serviceframework/symbian/dll/databasemanagerserver_dll.pro \
                  serviceframework/symbian/exe/databasemanagerserver_exe.pro \
                  $$SUBDIRS
        CONFIG += ordered
    }
}

contains(mobility_modules,bearer): SUBDIRS += bearer
contains(mobility_modules,location): SUBDIRS += location
contains(mobility_modules,multimedia): SUBDIRS += multimedia
contains(mobility_modules,contacts): SUBDIRS += contacts
contains(mobility_modules,telephony): SUBDIRS += telephony

contains(mobility_modules,publishsubscribe) {
    SUBDIRS += publishsubscribe
    symbian {
        SUBDIRS += publishsubscribe/psmapperserver_symbian/pspathmapperserver.pro
    }
}

contains(mobility_modules,systeminfo): SUBDIRS += systeminfo
contains(mobility_modules,versit): SUBDIRS += versit

# Versit depends on Contacts
versit.subdir=versit
versit.target=sub-versit
versit.depends=contacts

contains(mobility_modules,messaging) {
    contains(qmf_enabled, yes)|wince*|win32|symbian|maemo6 {
        !win32-g++:SUBDIRS += messaging
    }
}

symbian {
    SUBDIRS += s60installs/s60installs.pro
}
