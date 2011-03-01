TEMPLATE = app
TARGET = querymessages

QT -= gui

include(../mobility_examples.pri)

CONFIG += mobility console
MOBILITY = messaging
INCLUDEPATH += ../../src/messaging

SOURCES += \
    main.cpp
    
symbian:TARGET.CAPABILITY = NetworkServices \
    LocalServices \
    ReadUserData \
    WriteUserData \
    UserEnvironment \
    ReadDeviceData \
    WriteDeviceData

