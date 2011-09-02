TEMPLATE = subdirs

include(../../staticconfig.pri)

contains(mobility_modules,contacts): SUBDIRS += contacts-tester
contains(mobility_modules,bearer) {
    symbian: SUBDIRS += bearerex
}
contains(mobility_modules,location) SUBDIRS += location-testing-tools

contains(mobility_modules,systeminfo): SUBDIRS += sysinfo-tester

contains(mobility_modules,connectivity) {

    SUBDIRS += \
    qllcpsocket \
    qllcpserver

    symbian: SUBDIRS += nfcsymbianbackend

}

contains(mobility_modules,messaging) {
    symbian: SUBDIRS += messagingex
}

contains(mobility_modules,publishsubscribe) {
    symbian: SUBDIRS += publishsubscribeex
}

