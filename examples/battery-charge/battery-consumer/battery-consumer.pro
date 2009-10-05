TEMPLATE = app
TARGET = battery-consumer
QT += declarative
INCLUDEPATH += ../../../context
include(../../examples.pri)
qtAddLibrary(QtContextFramework)
SOURCES = main.cpp \
    batterycharge.cpp
RESOURCES = battery-consumer.qrc
HEADERS += batterycharge.h
