TEMPLATE = app
TARGET = battery-consumer
QT += declarative
INCLUDEPATH += ../../../context
DEPENDPATH += ../../../context
include(../../examples.pri)
qtAddLibrary(QtContextFramework)
SOURCES = main.cpp
RESOURCES = battery-consumer.qrc
