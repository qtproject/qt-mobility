TEMPLATE = subdirs

CONFIG += ordered

include(../../common.pri)

#wince*:SUBDIRS += wince
symbian {
    SUBDIRS += symbian
    contains(build_unit_tests, yes):SUBDIRS += symbian/tsrc
}

# perhaps this should just use the configure test
maemo6|meego {
    SUBDIRS += mkcal
}

maemo5 {
    contains(maemo5-calendar_enabled, yes) {
        SUBDIRS += maemo5
        contains(build_unit_tests, yes):SUBDIRS += maemo5/tsrc
    } else {
        message("Unable to build organizer backend plugin for maemo5!  Please install calendar-backend-dev to build this plugin!")
    }
}
simulator:SUBDIRS += simulator

# To start with
SUBDIRS += skeleton
