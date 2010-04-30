TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../src/serviceframework
HEADERS += bluetoothtransferplugin.h \
           bluetoothtransfer.h
SOURCES += bluetoothtransferplugin.cpp \
           bluetoothtransfer.cpp
TARGET = serviceframework_bluetoothtransferplugin
DESTDIR = .

include(../examples.pri)
CONFIG += mobility
MOBILITY = serviceframework

symbian {
    load(data_caging_paths)
    pluginDep.sources = serviceframework_bluetoothtransferplugin.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR    
    DEPLOYMENT += pluginDep

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = LocalServices Location NetworkServices ReadUserData WriteUserData UserEnvironment
}

xml.path = $$DESTDIR/xmldata
xml.files = bluetoothtransferservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
