TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../../src/serviceframework
PLUGIN_TYPE = serviceframework
HEADERS += voipdialer.h \
           voipdialerplugin.h

SOURCES += voipdialer.cpp \
           voipdialerplugin.cpp
QT += gui
TARGET = serviceframework_voipdialerservice

contains(QT_CONFIG, declarative):DEFINES += DECLARATIVE

include(../../mobility_examples.pri)
CONFIG += mobility
MOBILITY = serviceframework

symbian {
    load(data_caging_paths)
    pluginDep.sources = serviceframework_voipdialerservice.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR
    DEPLOYMENT += pluginDep

    xmlautoimport.path = /private/2002AC7F/import/
    xmlautoimport.sources = voipdialerservice.xml
    DEPLOYMENT += xmlautoimport

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = LocalServices Location NetworkServices ReadUserData WriteUserData UserEnvironment
    load(armcc_warnings)
}

#not needed for symbian due to autoimport above
xml.path = $$QT_MOBILITY_EXAMPLES/xmldata
xml.files = voipdialerservice.xml
INSTALLS += xml

contains(MEEGO_EDITION,harmattan) {
    target.path = /usr/lib/qt4/plugins/serviceframework/
    INSTALLS += target

    xml.path = /opt/declarative-sfw-dialer/bin/xmldata
    xml.files = voipdialerservice.xml
    INSTALLS += xml
}

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

