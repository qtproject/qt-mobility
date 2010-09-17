TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../../src/serviceframework
PLUGIN_TYPE = serviceframework
HEADERS += landlinedialer.h \
           landlinedialerplugin.h

SOURCES += landlinedialer.cpp \
           landlinedialerplugin.cpp
QT += gui
TARGET = serviceframework_landlinedialerservice

contains(QT_CONFIG, declarative):DEFINES += DECLARATIVE

include(../../examples.pri)
CONFIG += mobility
MOBILITY = serviceframework

symbian {
    load(data_caging_paths)
    pluginDep.sources = serviceframework_landlinedialerservice.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR
    DEPLOYMENT += pluginDep

    xmlautoimport.sources = landlinedialerservice.xml
    xmlautoimport.path = /private/2002AC7F/import/
    DEPLOYMENT += xmlautoimport

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    load(armcc_warnings)
}

# not needed for Symbian due to auto import above
xml.path = $$QT_MOBILITY_EXAMPLES/xmldata
xml.files = landlinedialerservice.xml
INSTALLS += xml
