include(../../common.pri)

TEMPLATE = app
TARGET = sendmessage

QT += gui

win32:debug {
    OUTPREFIX=Debug
} else:win32 {
    OUTPREFIX=Release
}

# Build against the messaging library
INCLUDEPATH += $$_PRO_FILE_PWD_/../../messaging
LIBS += -L$$OUT_PWD/../../messaging/$$OUTPREFIX -lQtMessaging

DEFINES += QMESSAGING_OPTIONAL QMESSAGING_OPTIONAL_FOLDER

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail


DEFINES += SRCDIR=\\\"$$_PRO_FILE_PWD_\\\"
}

HEADERS += \
    messagesender.h

SOURCES += \
    messagesender.cpp\
    main.cpp

