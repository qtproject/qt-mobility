TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../../src/serviceframework
TARGET = serviceframework_landlinedialerservice
include(../../../common.pri)

QT += gui

# Input 
HEADERS += landlinedialer.h landlinedialerplugin.h
SOURCES += landlinedialer.cpp landlinedialerplugin.cpp

qtAddLibrary(QtServiceFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

xml.path = $$DESTDIR/xmldata
xml.files = landlinedialerservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
