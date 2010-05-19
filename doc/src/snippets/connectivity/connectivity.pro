TARGET = connectivity

INCLUDEPATH += ../../../../src/connectivity
include(../../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

SOURCES = main.cpp \
    devicediscovery.cpp

HEADERS =

CONFIG += strict_flags
