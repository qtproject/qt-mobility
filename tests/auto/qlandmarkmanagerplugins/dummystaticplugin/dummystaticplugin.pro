TEMPLATE = lib
CONFIG += plugin static testplugin
TARGET = $$qtLibraryTarget(landmarks_testdummystatic)
PLUGIN_TYPE=landmarks

include(../../../../common.pri)

INCLUDEPATH += ../../../../src/location \
               ../../../../src/location/landmarks

CONFIG += mobility
MOBILITY = location

HEADERS += dummystaticplugin.h
