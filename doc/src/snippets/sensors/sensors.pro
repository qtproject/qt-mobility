TEMPLATE=app
TARGET=sensorsdocsnippet
include(../../../../features/basic_examples_setup.pri)
INCLUDEPATH += ../../../../src/sensors
SOURCES+=main.cpp\
    creating.cpp\
    start.cpp\
    plugin.cpp
HEADERS+=mybackend.h
LIBS+=-rdynamic
CONFIG+=mobility
MOBILITY+=sensors
CONFIG+=strict_flags
