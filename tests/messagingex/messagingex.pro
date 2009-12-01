TEMPLATE = app
TARGET = MessagingEx 

QT += gui

include(../../common.pri)

# Build against the messaging library
INCLUDEPATH += $$SOURCE_DIR/src/messaging
qtAddLibrary(QtMessaging)

HEADERS += \
    messagingex.h

SOURCES += \
    main.cpp \
    messagingex.cpp
    
FORMS += mmsreceiveddialog.ui \
    messagingex.ui \
    smsreceiveddialog.ui \
    accountdialog.ui

symbian:TARGET.CAPABILITY = NetworkServices \
    LocalServices \
    ReadUserData \
    WriteUserData \
    UserEnvironment \
    ReadDeviceData \
    WriteDeviceData
    
