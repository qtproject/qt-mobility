TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

!symbian {
    maemo6 {
        SUBDIRS += qsparql
    } else {
        SUBDIRS += sqlite
    }
}

symbian {
SUBDIRS += symbian_landmarks
}
