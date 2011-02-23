TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtmedia_pulse)
PLUGIN_TYPE = audio
include (../../../common.pri)
INCLUDEPATH += ../../../src/multimedia \
    ../../../src/multimedia/audio
CONFIG += mobility
MOBILITY = multimedia

CONFIG += link_pkgconfig
PKGCONFIG += libpulse

# Input
HEADERS += qpulseaudioplugin.h \
           qaudiodeviceinfo_pulse.h \
           qaudiooutput_pulse.h \
           qaudioinput_pulse.h \
           qpulseaudioengine.h \
           qpulsehelpers.h

SOURCES += qpulseaudioplugin.cpp \
           qaudiodeviceinfo_pulse.cpp \
           qaudiooutput_pulse.cpp \
           qaudioinput_pulse.cpp \
           qpulseaudioengine.cpp \
           qpulsehelpers.cpp
