TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../../src/serviceframework
TARGET = serviceframework_landlinedialerservice
include(../../examples.pri)

QT += gui

# Input 
HEADERS += landlinedialer.h landlinedialerplugin.h
SOURCES += landlinedialer.cpp landlinedialerplugin.cpp

CONFIG += mobility
MOBILITY = serviceframework

symbian {
    TARGET.CAPABILITY = LocalServices Location NetworkServices ReadUserData UserEnvironment WriteUserData
}

xml.path = $$DESTDIR/xmldata
xml.files = landlinedialerservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
