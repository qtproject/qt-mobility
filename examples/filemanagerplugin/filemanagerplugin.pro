TEMPLATE = lib
CONFIG += plugin
PLUGIN_TYPE=serviceframework
INCLUDEPATH += ../../src/serviceframework
HEADERS += filemanagerplugin.h \
           filemanagerstorage.h \
           filemanagertransfer.h
SOURCES += filemanagerplugin.cpp \
           filemanagerstorage.cpp \
           filemanagertransfer.cpp
TARGET = serviceframework_filemanagerplugin
DESTDIR = .

include(../examples.pri)
CONFIG += mobility
MOBILITY = serviceframework

symbian {
    load(data_caging_paths)
    pluginDep.sources = serviceframework_filemanagerplugin.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR    
    DEPLOYMENT += pluginDep

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = LocalServices Location NetworkServices ReadUserData WriteUserData UserEnvironment
}

xml.path = $$QT_MOBILITY_EXAMPLES/xmldata
xml.files = filemanagerservice.xml
INSTALLS += xml
