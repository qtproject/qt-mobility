TEMPLATE = app
TARGET = battery-consumer
QT += declarative
INCLUDEPATH += ../../../context
include(../../../common.pri)
LIBS += -lQtContextFramework
SOURCES = main.cpp \
    batterycharge.cpp
RESOURCES = battery-consumer.qrc
HEADERS += batterycharge.h
