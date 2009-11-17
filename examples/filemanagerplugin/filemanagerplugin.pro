TEMPLATE = lib
CONFIG += plugin
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
qtAddLibrary(QtServiceFramework)

symbian {
    load(data_caging_paths)
    pluginDep.sources = serviceframework_filemanagerplugin.dll
    pluginDep.path = $$QT_PLUGINS_BASE_DIR    
    DEPLOYMENT += pluginDep

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
}

xml.path = $$DESTDIR/xmldata
xml.files = filemanagerservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
