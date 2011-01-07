include(../../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,sensors): SUBDIRS += sensors
contains(mobility_modules,serviceframework): SUBDIRS += serviceframework
contains(mobility_modules,publishsubscribe): SUBDIRS += publishsubscribe
contains(mobility_modules,multimedia): SUBDIRS += multimedia
contains(mobility_modules,organizer):contains(mobility_modules,versit) SUBDIRS += organizer
contains(mobility_modules,systeminfo): SUBDIRS += systeminfo
contains(mobility_modules,gallery): SUBDIRS += gallery
contains(mobility_modules,feedback): SUBDIRS += feedback
contains(mobility_modules,contacts):contains(mobility_modules,versit) SUBDIRS += contacts
contains(mobility_modules,location): SUBDIRS += location
contains(mobility_modules,connectivity): SUBDIRS += connectivity
contains(qmf_enabled, yes)|wince*|win32|symbian|maemo5|maemo6 {
    contains(mobility_modules,messaging) {
        !win32-g++:SUBDIRS += messaging
    }
}
