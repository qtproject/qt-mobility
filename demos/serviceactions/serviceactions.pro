TEMPLATE = app
TARGET = serviceactions 

QT += gui

include(../demos.pri)

CONFIG += mobility
MOBILITY = messaging
INCLUDEPATH += ../../src/messaging

HEADERS += \
    mainwindow.h \
    attachmentlistwidget.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    attachmentlistwidget.cpp
    
symbian:TARGET.CAPABILITY = NetworkServices \
    LocalServices \
    ReadUserData \
    WriteUserData \
    UserEnvironment \
    ReadDeviceData \
    WriteDeviceData
    
