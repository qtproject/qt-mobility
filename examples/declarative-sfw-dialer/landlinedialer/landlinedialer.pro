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

include(../../mobility_examples.pri)
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
    TARGET.CAPABILITY = LocalServices Location NetworkServices ReadUserData WriteUserData UserEnvironment
    load(armcc_warnings)
}

# not needed for Symbian due to auto import above
xml.path = $$QT_MOBILITY_EXAMPLES/xmldata
xml.files = landlinedialerservice.xml
INSTALLS += xml

contains(MEEGO_EDITION,harmattan) {
    xml.path = /opt/declarative-sfw-dialer/bin/xmldata
    xml.files = landlinedialerservice.xml
    INSTALLS += xml

    target.path = /usr/lib/qt4/plugins/serviceframework/
    INSTALLS += target
}

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog



