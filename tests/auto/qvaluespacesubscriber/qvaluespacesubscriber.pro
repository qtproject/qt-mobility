TEMPLATE = subdirs

SUBDIRS = tst_qvaluespacesubscriber
!symbian {
    SUBDIRS += lackey tst_qvaluespacesubscriber_oop
}
