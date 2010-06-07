TEMPLATE = app
TARGET = serviceactions 

QT += gui

include(../../examples/examples.pri)

CONFIG += mobility
MOBILITY = messaging
INCLUDEPATH += ../../src/messaging

symbian|win32|wince*|maemo5|mac {
} else {
    # Temporarily link against local qtopiamail lib (should be part of the platform)
    LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
}

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
    
