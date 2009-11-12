TEMPLATE = app
TARGET = battery-consumer
QT += declarative
INCLUDEPATH += ../../../context
DEPENDPATH += ../../../context
include(../../examples.pri)
qtAddLibrary(QtPublishSubscribe)
SOURCES = main.cpp
RESOURCES = battery-consumer.qrc
OTHER_FILES += battery-meter.qml
