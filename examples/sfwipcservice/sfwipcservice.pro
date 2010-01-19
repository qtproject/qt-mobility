TEMPLATE = app
TARGET = sfwipcservice
INCLUDEPATH += ../../src/serviceframework
INCLUDEPATH += ../../src/serviceframework/ipc

include(../examples.pri)

QT += core network

# Input 
SOURCES += main.cpp

CONFIG += mobility
MOBILITY = serviceframework
