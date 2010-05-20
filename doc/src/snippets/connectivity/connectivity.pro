TARGET = connectivity

INCLUDEPATH += ../../../../src/connectivity
include(../../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

SOURCES = main.cpp \
    devicediscovery.cpp \
    servicediscovery.cpp

HEADERS =

CONFIG += strict_flags
