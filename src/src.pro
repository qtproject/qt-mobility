include($$QT_MOBILITY_BUILD_TREE/config.pri)

TEMPLATE = subdirs
SUBDIRS += serviceframework
symbian:SUBDIRS += serviceframework/symbian/dll/databasemanagerserver_dll.pro \
                   serviceframework/symbian/exe/databasemanagerserver_exe.pro

SUBDIRS += bearer location contacts multimedia publishsubscribe systeminfo

contains(qmf_enabled, yes)|wince*|win32|symbian|maemo {
    SUBDIRS += messaging
}
