# config.pri specifies the configure options

!include($$QT_MOBILITY_BUILD_TREE/config.pri) {
    error("Please run configure script");
}

lessThan(QT_MAJOR_VERSION, 4) {
    error(Qt Mobility requires Qt 4.5 or higher. Qt $${QT_VERSION} was found.);
}

contains(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 5) {
    error(Qt Mobility requires Qt 4.5 or higher. Qt $${QT_VERSION} was found.);
}


#generate prf file for Qt integration
PRF_OUTPUT=$${QT_MOBILITY_BUILD_TREE}/features/mobility.prf

system(echo MOBILITY_PREFIX=$${QT_MOBILITY_PREFIX} > $$PRF_OUTPUT)
system(echo MOBILITY_INCLUDE=$${QT_MOBILITY_INCLUDE} >> $$PRF_OUTPUT)
system(echo MOBILITY_LIB=$${QT_MOBILITY_LIB} >> $$PRF_OUTPUT)

unix:!symbian:system(cat $${QT_MOBILITY_SOURCE_TREE}/features/mobility.prf.template >> $$PRF_OUTPUT)
win32:system(type $${QT_MOBILITY_SOURCE_TREE}\features\mobility.prf.template >> $$PRF_OUTPUT)
symbian:system(type $${QT_MOBILITY_SOURCE_TREE}\features\mobility.prf.template >> $$PRF_OUTPUT)


# install feature file
feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
feature.files = $$QT_MOBILITY_BUILD_TREE/features/mobility.prf

INSTALLS += feature

TEMPLATE = subdirs
CONFIG+=ordered

SUBDIRS += serviceframework
symbian:SUBDIRS += serviceframework/symbian/dll/databasemanagerserver_dll.pro \
                   serviceframework/symbian/exe/databasemanagerserver_exe.pro

SUBDIRS += bearer location contacts multimedia context systeminfo tools plugins
SUBDIRS += doc   #built documentation snippets


contains(qmf_enabled, yes)|wince*|win32|symbian|maemo {
    SUBDIRS += messaging
}


contains(build_unit_tests, yes):SUBDIRS+=tests
contains(build_examples, yes):SUBDIRS+=examples

include(doc/doc.pri)

# install Qt style headers
qtmheaders.path = $${QT_MOBILITY_INCLUDE}
qtmheaders.files = $${QT_MOBILITY_BUILD_TREE}/include/*

INSTALLS += qtmheaders

OTHER_FILES += doc/src/*.qdoc doc/src/examples/*.qdoc
