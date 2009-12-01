TEMPLATE = app
TARGET = battery-subscriber
QT += declarative
INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe
include(../../examples.pri)
qtAddLibrary(QtPublishSubscribe)
SOURCES = main.cpp
RESOURCES = battery-subscriber.qrc
OTHER_FILES += battery-meter.qml
