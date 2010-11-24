TEMPLATE = subdirs

symbian {

    SUBDIRS += qnearfieldmanager qllcpserver qllcpsockettype1 qllcpsockettype2 qnearfieldtagtype1 qnearfieldtagtype2 qnearfieldtagtype3 qnearfieldtagtype4
    contains(mobility_modules, serviceframework): SUBDIRS += nfcserviceprovider
}
