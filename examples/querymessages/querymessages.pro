TEMPLATE = app
TARGET = querymessages

QT += gui

include(../examples.pri)

CONFIG += mobility
MOBILITY = messaging
INCLUDEPATH += ../../src/messaging

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
}

SOURCES += \
    main.cpp
    
symbian:TARGET.CAPABILITY = NetworkServices \
    LocalServices \
    ReadUserData \
    WriteUserData \
    UserEnvironment \
    ReadDeviceData \
    WriteDeviceData

