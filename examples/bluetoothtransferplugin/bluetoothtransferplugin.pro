TEMPLATE = lib
CONFIG += plugin
PLUGIN_TYPE=serviceframework
INCLUDEPATH += ../../src/serviceframework

HEADERS += bluetoothtransferplugin.h \
           bluetoothtransfer.h
SOURCES += bluetoothtransferplugin.cpp \
           bluetoothtransfer.cpp
TARGET = $$qtLibraryTarget(serviceframework_bluetoothtransferplugin)
DESTDIR = .

include(../examples.pri)
CONFIG += mobility
MOBILITY = serviceframework

symbian {
    load(data_caging_paths)
    pluginDep.sources = $${TARGET}.dll
    pluginDep.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}    
    DEPLOYMENT += pluginDep

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = LocalServices Location NetworkServices ReadUserData WriteUserData UserEnvironment
}

xml.path = $$QT_MOBILITY_PREFIX/bin/xmldata
xml.files = bluetoothtransferservice.xml
INSTALLS += xml
