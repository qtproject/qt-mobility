TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qsimulatorengine)
PLUGIN_TYPE=mediaservice

include(../../../common.pri)
INCLUDEPATH+=../../../src/multimedia \
             ../../../src/multimedia/video \
             ../../../src/multimedia/audio \
             ../../../src/mobilitysimulator

CONFIG += mobility
MOBILITY = multimedia

DEPENDPATH += .

# Input
HEADERS += \
    qsimulatormultimediaconnection_p.h \
    qsimulatormultimediadata_p.h \
    qsimulatorserviceplugin.h

SOURCES += \
    qsimulatormultimediaconnection.cpp \
    qsimulatormultimediadata.cpp \
    qsimulatorserviceplugin.cpp \

include(camera/simulatorcamera.pri)
