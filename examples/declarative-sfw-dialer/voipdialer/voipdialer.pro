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
DESTDIR = .

include(../../examples.pri)
CONFIG += mobility
MOBILITY = serviceframework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

xml.path = $$QT_MOBILITY_PREFIX/bin/xmldata
xml.files = voipdialerservice.xml
INSTALLS += xml
