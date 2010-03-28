TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../../src/serviceframework
TARGET = serviceframework_voipdialerservice
include(../../examples.pri)

QT += gui

# Input 
HEADERS += voipdialer.h voipdialerplugin.h
SOURCES += voipdialer.cpp voipdialerplugin.cpp

CONFIG += mobility
MOBILITY = serviceframework

symbian {
    TARGET.CAPABILITY = LocalServices Location NetworkServices ReadUserData UserEnvironment WriteUserData
}

xml.path = $$DESTDIR/xmldata
xml.files = ../voipdialerservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
