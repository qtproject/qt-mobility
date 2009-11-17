TEMPLATE = app
TARGET = sendmessage

QT += gui

include(../examples.pri)

qtAddLibrary(QtMessaging)
INCLUDEPATH += ../../src/messaging

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
}

HEADERS += \
    messagesender.h

SOURCES += \
    messagesender.cpp\
    main.cpp

