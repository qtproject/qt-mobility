TEMPLATE = subdirs

SUBDIRS = tst_qvaluespacesubscriber
!symbian {
    SUBDIRS += lackey
    !maemo5:SUBDIRS += tst_qvaluespacesubscriber_oop
}
