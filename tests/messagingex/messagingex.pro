TEMPLATE = app
TARGET = MessagingEx 

QT += gui

include(../../common.pri)
maemo5:include(../../examples/examples.pri)

# Build against the messaging library
INCLUDEPATH += $$SOURCE_DIR/src/messaging
CONFIG += mobility
MOBILITY = messaging

maemo5|maemo6 {
    QMAKE_CXXFLAGS+=-DUSE_TABBED_LAYOUT
    QMAKE_RPATHDIR+=$$SOURCE_DIR/lib
}

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
    WriteDeviceData \
    SwEvent
    
