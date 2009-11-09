TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../../serviceframework
TARGET = serviceframework_voipdialerservice
include(../../../common.pri)

QT += gui

# Input 
HEADERS += voipdialer.h voipdialerplugin.h
SOURCES += voipdialer.cpp voipdialerplugin.cpp

qtAddLibrary(QtServiceFramework)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

xml.path = $$DESTDIR/xmldata
xml.files = voipdialerservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
