TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

!symbian {
SUBDIRS += sqlite
}

symbian {
SUBDIRS += symbian_landmarks
}
