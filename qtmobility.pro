# config.pri specifies the configure options
!include(config.pri) {
    error("Please run configure script");
}

#generate prf file for Qt integration

PRF_OUTPUT=features/mobility.prf

unix {
    system(echo MOBILITY_PREFIX=$${QT_MOBILITY_PREFIX} > $$PRF_OUTPUT)
    system(echo MOBILITY_INCLUDE=$${QT_MOBILITY_INCLUDE} >> $$PRF_OUTPUT)
    system(echo MOBILITY_LIB=$${QT_MOBILITY_LIB} >> $$PRF_OUTPUT)
    system(cat features/mobility.prf.template >> $$PRF_OUTPUT)
}

win32 {
}
win32:system((echo MOBILITY_PREFIX=$${QT_MOBILITY_PREFIX} & echo MOBILITY_INCLUDE=$${QT_MOBILITY_INCLUDE} & type features\mobility.prf.template) > features\mobility.prf)

# install feature file
feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
feature.files = features/mobility.prf

INSTALLS += feature

TEMPLATE = subdirs
CONFIG+=ordered

SUBDIRS += serviceframework bearer location context tools

contains(build_unit_tests, yes):SUBDIRS+=tests
contains(build_examples, yes):SUBDIRS+=examples
