
# Copyright (c) 2010 Nokia Corporation.

TARGET = qwhowhere
TEMPLATE = app
QT += network

CONFIG += mobility
MOBILITY = location \
    bearer \
    contacts \
    messaging


INCLUDEPATH += ../../src/location \
		../../src/bearer \
		../../src/contacts \
		../../src/contacts/details \
		../../src/contacts/filters \
		../../src/messaging

include(../examples.pri)

SOURCES += message.cpp \
    contactsdialog.cpp \
    fetchmap.cpp \
    pictureitem.cpp \
    main.cpp \
    mainwindow.cpp
HEADERS += message.h \
    contactsdialog.h \
    fetchmap.h \
    pictureitem.h \
    mainwindow.h

RESOURCES = resources.qrc

symbian:TARGET.CAPABILITY = NetworkServices \
    Location \
    ReadUserData \
    WriteUserData \
    LocalServices \
    UserEnvironment \
    ReadDeviceData \
    WriteDeviceData
    
    
