TEMPLATE = subdirs

symbian {

    SUBDIRS += qnearfieldmanager qllcpserver qllcpsocketlocal qllcpsocketremote qllcpsockettype2
    contains(mobility_modules, serviceframework): SUBDIRS += nfcserviceprovider
}
