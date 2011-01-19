TEMPLATE = subdirs

symbian {

    SUBDIRS += qnearfieldmanager qllcpserver qllcpsockettype2 qllcpsocketlocal qllcpsocketremote  qnearfieldtagtype1 qnearfieldtagtype2 qnearfieldtagtype3 qnearfieldtagtype4
    contains(mobility_modules, serviceframework): SUBDIRS += nfctestserviceprovider
}
