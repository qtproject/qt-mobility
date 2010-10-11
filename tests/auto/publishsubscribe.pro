include(../../staticconfig.pri)

requires(contains(mobility_modules,publishsubscribe))

TEMPLATE = subdirs

SUBDIRS += qvaluespace \
       qvaluespacepublisher \
       qvaluespacesubscriber \
       qcrmlparser

unix|win32 {
    !symbian:!maemo6:!maemo5: SUBDIRS+= \
        qsystemreadwritelock \
        qsystemreadwritelock_oop
}

unix:!symbian:!maemo6:!maemo5 {
    SUBDIRS+= \
           qpacket \
           qmallocpool \
           qpacketprotocol
}

