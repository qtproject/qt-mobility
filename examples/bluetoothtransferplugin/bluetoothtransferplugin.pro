TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../servicefw/serviceframework/servicehandler \
               ../../servicefw/serviceframework/serviceresolution \
               ../../servicefw/serviceframework
HEADERS += bluetoothtransferplugin.h \
           bluetoothtransfer.h
SOURCES += bluetoothtransferplugin.cpp \
           bluetoothtransfer.cpp
TARGET = serviceframework_bluetoothtransferplugin
DESTDIR = .

include(../../common.pri)
LIBS += -lQtServiceFramework

symbian {
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = ALL -TCB
}

xml.path = $$DESTDIR/xmldata
xml.files = bluetoothtransferservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
