include(../../staticconfig.pri)

requires(contains(mobility_modules,publishsubscribe))

TEMPLATE = subdirs

SUBDIRS += qvaluespace \
       qvaluespacepublisher \
       qvaluespacesubscriber \
       qcrmlparser

unix|win32 {
    !symbian:!maemo6:!maemo5:!meego: SUBDIRS+= \
        qsystemreadwritelock \
        qsystemreadwritelock_oop
}

unix:!symbian:!maemo6:!maemo5:!meego {
    SUBDIRS+= \
           qpacket \
           qmallocpool \
           qpacketprotocol
}

