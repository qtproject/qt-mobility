TEMPLATE      = lib
CONFIG       += plugin testplugin
INCLUDEPATH  += ../../src/serviceframework
HEADERS       = sampleserviceplugin.h
SOURCES       = sampleserviceplugin.cpp
TARGET        = tst_sfw_sampleserviceplugin
DESTDIR = .

include(../../common.pri)
CONFIG += mobility
MOBILITY = serviceframework

symbian {
    load(data_caging_paths)
    pluginDep.sources = tst_sfw_sampleserviceplugin.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR/plugins

    DEPLOYMENT += pluginDep
    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
}

xml.path = $$DESTDIR/xmldata
xml.files = xml/sampleservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml

