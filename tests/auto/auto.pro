TEMPLATE = subdirs
SUBDIRS += qvaluespace \
           qvaluespaceprovider \
           qvaluespacesubscriber \
           qmallocpool \
           qpacket \
           qpacketprotocol

unix|win32 {
    !symbian: SUBDIRS+= \
        qsystemreadwritelock \
        qsystemreadwritelock_oop
}
