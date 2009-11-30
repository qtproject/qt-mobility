include($$QT_MOBILITY_BUILD_TREE/config.pri)

TEMPLATE = subdirs
SUBDIRS += global serviceframework
symbian {
    SUBDIRS = serviceframework/symbian/dll/databasemanagerserver_dll.pro \
              serviceframework/symbian/exe/databasemanagerserver_exe.pro \
              $$SUBDIRS
    CONFIG += ordered
}

SUBDIRS += bearer location contacts multimedia publishsubscribe versit sensors
!maemo:SUBDIRS+=systeminfo

# Versit depends on Contacts
versit.subdir=versit
versit.target=sub-versit
versit.depends=contacts

contains(qmf_enabled, yes)|wince*|win32|symbian|maemo {
    !win32-g++:SUBDIRS += messaging
}

