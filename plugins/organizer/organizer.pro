TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

#wince*:SUBDIRS += wince
symbian {
    SUBDIRS += symbian
    contains(build_unit_tests, yes):SUBDIRS += symbian/tsrc
}
maemo6:SUBDIRS += maemo6
maemo5:SUBDIRS += maemo5

# To start with
SUBDIRS += skeleton
