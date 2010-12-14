TEMPLATE = subdirs

symbian {

    SUBDIRS += qnearfieldmanager qllcpserver qllcpsocketlocal qllcpsocketremote 
    contains(mobility_modules, serviceframework): SUBDIRS += nfcserviceprovider
}
