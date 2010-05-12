TEMPLATE=app
TARGET=sensors
include(../../../../common.pri)
INCLUDEPATH += ../../../../src/sensors
DESTDIR=$$PWD
SOURCES+=main.cpp\
    creating.cpp\
    start.cpp\
    plugin.cpp
HEADERS+=mybackend.h
LIBS+=-rdynamic
CONFIG+=mobility
MOBILITY+=sensors
CONFIG+=strict_flags
