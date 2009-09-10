# config.pri specifies the configure options
!include(config.pri) {
    error("Please run configure script");
}

#generate prf file for Qt integration

PRF_OUTPUT=features/mobility.prf

system(echo MOBILITY_PREFIX=$${QT_MOBILITY_PREFIX} > $$PRF_OUTPUT)
system(echo MOBILITY_INCLUDE=$${QT_MOBILITY_INCLUDE} >> $$PRF_OUTPUT)
system(echo MOBILITY_LIB=$${QT_MOBILITY_LIB} >> $$PRF_OUTPUT)

unix:!symbian:system(cat features/mobility.prf.template >> $$PRF_OUTPUT)
win32:system(type features\mobility.prf.template >> features\mobility.prf)
symbian:system(type features\mobility.prf.template >> features\mobility.prf)


# install feature file
feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
feature.files = features/mobility.prf

INSTALLS += feature

TEMPLATE = subdirs
CONFIG+=ordered

SUBDIRS += serviceframework bearer location context systeminfo tools

contains(build_unit_tests, yes):SUBDIRS+=tests
contains(build_examples, yes):SUBDIRS+=examples

# install Qt style headers
qtmheaders.path = $${QT_MOBILITY_INCLUDE}
qtmheaders.files = include/*

unix:INSTALLS += qtmheaders
