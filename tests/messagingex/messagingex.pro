TEMPLATE = app
TARGET = MessagingEx 

QT += gui

include(../../common.pri)

# Build against the messaging library
INCLUDEPATH += $$SOURCE_DIR/messaging
LIBS += -lQtMessaging

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
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
    

symbian:TARGET.CAPABILITY = All -TCB -AllFiles -DRM