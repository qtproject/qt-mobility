TEMPLATE = subdirs
SUBDIRS += qvaluespace \
           qvaluespaceobject \
           qvaluespaceitem \
           qmallocpool
unix:SUBDIRS += qpacket \
                qpacketprotocol \
                qsystemreadwritelock \
                qsystemreadwritelock_oop
