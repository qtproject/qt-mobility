TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

!symbian {
SUBDIRS += qsparql
}

symbian {
SUBDIRS += symbian_landmarks
}
