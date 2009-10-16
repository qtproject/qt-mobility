TEMPLATE = subdirs
SUBDIRS += qvaluespace \
           qvaluespaceprovider \
           qvaluespacesubscriber \
	   qcrmlparser

unix|win32 {
    !symbian:!maemo: SUBDIRS+= \
        qsystemreadwritelock \
        qsystemreadwritelock_oop
}

unix:!symbian:!maemo: {
    SUBDIRS+= \
           qpacket \
           qmallocpool \
           qpacketprotocol
}
