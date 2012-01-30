TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

SUBDIRS += backuphandler vcardpreserver

symbian {
    #versit symbian plugins moved to contactsrv package
   !exists($${EPOCROOT}epoc32/release/winscw/udeb/z/system/install/series60v5.4.sis){
    SUBDIRS += symbian
    }
}
