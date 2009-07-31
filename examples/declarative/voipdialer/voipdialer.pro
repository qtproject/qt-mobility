TEMPLATE = lib
CONFIG += plugin
TARGET=sfw-kinetic-example
INCLUDEPATH += ../../../serviceframework
TARGET = serviceframework_voipdialerservice

include(../../../common.pri)

QT += gui declarative

# Input 
HEADERS += ../sfwexample.h
SOURCES += ../sfwexample.cpp \
           ../main.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

xml.path = $$DESTDIR/xmldata
xml.files = voipdialerservice.xml
xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += xml
build_pass:ALL_DEPS+=install_xml
