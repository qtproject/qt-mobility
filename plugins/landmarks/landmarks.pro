TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

!symbian {
    maemo6 {
        contains(maemo6-landmarks_enabled, yes): SUBDIRS += qsparql
    } else {
        SUBDIRS += sqlite
    }
}

symbian {
SUBDIRS += symbian_landmarks
}
