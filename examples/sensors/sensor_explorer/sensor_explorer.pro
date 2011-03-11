TEMPLATE=app
TARGET=sensor_explorer
include(../../mobility_examples.pri)
QT=core gui
CONFIG+=mobility
MOBILITY+=sensors
INCLUDEPATH += ../../../src/sensors

FORMS=\
    explorer.ui

HEADERS=\
    explorer.h

SOURCES=\
    explorer.cpp\
    main.cpp

CONFIG+=strict_flags

maemo5:DEFINES+=MAEMO5

