TEMPLATE = app
TARGET = battery-consumer
QT += declarative
INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe
include(../../examples.pri)
qtAddLibrary(QtPublishSubscribe)
SOURCES = main.cpp
RESOURCES = battery-consumer.qrc
OTHER_FILES += battery-meter.qml
