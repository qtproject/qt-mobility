HEADERS = bearercloud.h \
          cloud.h

SOURCES = main.cpp \
          bearercloud.cpp \
          cloud.cpp

RESOURCES = icons.qrc

TARGET = bearercloud

QT = core gui network

INCLUDEPATH += ../../bearer

include(../../common.pri)

LIBS += -lbearer

CONFIG += console
