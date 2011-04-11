TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

SUBDIRS += backuphandler vcardpreserver

symbian {
    SUBDIRS += symbian
}
