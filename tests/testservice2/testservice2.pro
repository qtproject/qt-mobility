TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../../serviceframework
HEADERS       = testserviceplugin.h testservice.h testserviceinterface.h
SOURCES       = testserviceplugin.cpp

TARGET        = tst_sfw_testservice2plugin
DESTDIR = .
CONFIG += testcase

include(../../common.pri)
qtAddLibrary(QtServiceFramework)

xml.path = $$DESTDIR/xmldata
xml.files = xml/testserviceplugin.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
