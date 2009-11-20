TEMPLATE      = lib
CONFIG       += plugin testplugin
INCLUDEPATH  += ../../src/serviceframework
HEADERS       = testserviceplugin.h testservice.h testserviceinterface.h
SOURCES       = testserviceplugin.cpp
TARGET        = tst_sfw_testservice2plugin
DESTDIR = .

include(../../common.pri)
qtAddLibrary(QtServiceFramework)

symbian {
    load(data_caging_paths)
    pluginDep.sources = tst_sfw_testservice2plugin.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR/plugins
	
    pluginDep2.sources = qmakepluginstubs/tst_sfw_testservice2plugin.qtplugin
    pluginDep2.path = $$QT_PLUGINS_BASE_DIR

    DEPLOYMENT += pluginDep \ pluginDep2
	
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
}


xml.path = $$DESTDIR/xmldata
xml.files = xml/testserviceplugin.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
