TEMPLATE = app
TARGET = querymessages

QT -= gui

include(../examples.pri)

CONFIG += mobility console
MOBILITY = messaging
INCLUDEPATH += ../../src/messaging

symbian|win32|wince*|maemo5|mac {
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

