TEMPLATE = subdirs
SUBDIRS += qvaluespace \
           qvaluespaceprovider \
           qvaluespacesubscriber \
           qmallocpool \
           qpacket \
           qpacketprotocol \
	   qcrmlparser

unix|win32 {
    !symbian: SUBDIRS+= \
        qsystemreadwritelock \
        qsystemreadwritelock_oop
}
