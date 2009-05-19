TEMPLATE      = lib
CONFIG       += plugin
INCLUDEPATH  += ../../servicefw/serviceframework/serviceresolution \
                ../../servicefw/serviceframework/servicehandler \
                ../../servicefw/serviceframework
HEADERS       = sampleserviceplugin.h
SOURCES       = sampleserviceplugin.cpp
TARGET        = tst_sfw_sampleserviceplugin
DESTDIR = .

CONFIG += testcase

include(../../common.pri)
LIBS += -lQtServiceFramework


xml.path = $$DESTDIR/xmldata
xml.files = xml/sampleservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
