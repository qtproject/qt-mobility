# config.pri specifies the configure options
!include(config.pri) {
    unix:error("Please run configure script");
    #wait until we have a script for windows
    #error("Please run configure script");
}

#generate prf file for Qt integration

unix:system((echo MOBILITY_PREFIX=$${QT_MOBILITY_PREFIX}; echo MOBILITY_INCLUDE=$${QT_MOBILITY_INCLUDE}; cat features/mobility.prf.template) > features/mobility.prf)
win32: message("Win 32 todo")

# install feature file
feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
feature.files = features/mobility.prf

INSTALLS += feature

TEMPLATE = subdirs
CONFIG+=ordered

SUBDIRS += serviceframework bearer location context tools

contains(build_unit_tests, yes):SUBDIRS+=tests
contains(build_examples, yes):SUBDIRS+=examples
