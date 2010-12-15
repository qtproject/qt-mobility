TEMPLATE = subdirs

include (../../../common.pri)
include (../../../staticconfig.pri)
        
SUBDIRS += test


meego: contains(gypsy_enabled, yes) {
       message(Building also gypsy mock library)
       SUBDIRS +=  gypsymock
}

