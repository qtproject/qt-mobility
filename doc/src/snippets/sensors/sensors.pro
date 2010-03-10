TEMPLATE=app
TARGET=sensors
include(../../../../common.pri)
INCLUDEPATH += ../../../../src/sensors
SOURCES+=main.cpp\
    creating.cpp\
    start.cpp\
    plugin.cpp
HEADERS+=myplugin.h
LIBS+=-rdynamic
CONFIG+=mobility
MOBILITY+=sensors
