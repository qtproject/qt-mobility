TEMPLATE = app
TARGET = writemessage

QT += gui

include(../examples.pri)

CONFIG += mobility
MOBILITY = messaging
INCLUDEPATH += ../../src/messaging

HEADERS += \
    messagesender.h

SOURCES += \
    messagesender.cpp\
    main.cpp

symbian:TARGET.CAPABILITY = NetworkServices \
    LocalServices \
    ReadUserData \
    WriteUserData \
    UserEnvironment \
    ReadDeviceData \
    WriteDeviceData
    
