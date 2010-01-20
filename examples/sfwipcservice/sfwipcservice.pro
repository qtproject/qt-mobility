TEMPLATE = app
TARGET = sfwipcservice
INCLUDEPATH += ../../src/serviceframework
INCLUDEPATH += ../../src/serviceframework/ipc

include(../examples.pri)

QT += core network

# Input 
SOURCES += main.cpp

CONFIG += mobility
MOBILITY = serviceframework

xml.path = $$DESTDIR/xmldata
xml.files = ipcexampleservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
