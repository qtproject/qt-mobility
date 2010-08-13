TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

contains(mobility_modules,contacts) {
    SUBDIRS += backuphandler
}
