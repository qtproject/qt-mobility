TARGET = connectivity

INCLUDEPATH += ../../../../src/connectivity/nfc
DEPENDPATH += ../../../../src/connectivity/nfc

INCLUDEPATH += ../../../../src/connectivity/bluetooth
DEPENDPATH += ../../../../src/connectivity/bluetooth
include(../../../../common.pri)

CONFIG += mobility
MOBILITY = connectivity

SOURCES = main.cpp \
    nfc.cpp \
    devicediscovery.cpp \
    servicediscovery.cpp

HEADERS =

CONFIG += strict_flags
