TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

symbian {
    SUBDIRS += symbian
    exists($${EPOCROOT}epoc32/data/z/system/install/Series60v5.2.sis) {
        SUBDIRS += symbiansim
    }
}
wince*:SUBDIRS += wince
maemo:SUBDIRS += qtcontacts-tracker
