TEMPLATE = app
TARGET = writemessage

QT += gui

include(../examples.pri)

CONFIG += mobility
MOBILITY = messaging
INCLUDEPATH += ../../src/messaging

symbian|win32|wince*|maemo5|maemo6|mac {
} else {
# Temporarily link against local qmfclient lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqmfclient
}

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
    
