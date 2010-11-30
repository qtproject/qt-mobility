TEMPLATE = subdirs

symbian {

    SUBDIRS += qnearfieldmanager qllcpserver qllcpsocketlocal qllcpsocketremote qllcpsockettype2 qnearfieldtagtype1 qnearfieldtagtype2 qnearfieldtagtype3 qnearfieldtagtype4
    contains(mobility_modules, serviceframework): SUBDIRS += nfcserviceprovider
}
