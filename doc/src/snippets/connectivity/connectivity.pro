TARGET = connectivity

INCLUDEPATH += ../../../../src/connectivity/nfc
include(../../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

SOURCES = main.cpp \
    nfc.cpp

HEADERS =

CONFIG += strict_flags
