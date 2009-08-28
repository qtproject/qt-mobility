TEMPLATE = app
TARGET = battery-consumer
QT += declarative
INCLUDEPATH += ../../../context
include(../../../common.pri)
LIBS += -lQtContextFramework
SOURCES = main.cpp
RESOURCES = battery-consumer.qrc
