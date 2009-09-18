TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../../serviceframework
HEADERS       = sampleserviceplugin2.h
SOURCES       = sampleserviceplugin2.cpp
TARGET        = tst_sfw_sampleserviceplugin2
DESTDIR = .

CONFIG += testcase

include(../../common.pri)
qtAddLibrary(QtServiceFramework)


xml.path = $$DESTDIR/xmldata
xml.files = xml/sampleservice2.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
