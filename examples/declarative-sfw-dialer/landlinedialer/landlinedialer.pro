TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../../src/serviceframework
PLUGIN_TYPE = serviceframework
TARGET = serviceframework_landlinedialerservice
include(../../examples.pri)

QT += gui

# Input 
HEADERS += landlinedialer.h landlinedialerplugin.h
SOURCES += landlinedialer.cpp landlinedialerplugin.cpp

CONFIG += mobility
MOBILITY = serviceframework

symbian {
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA=1
    load(armcc_warnings)
}

xml.path = $$QT_MOBILITY_EXAMPLES/xmldata
xml.files = landlinedialerservice.xml
INSTALLS += xml
