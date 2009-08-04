TEMPLATE = app
TARGET = keepintouch

QT += gui

# Build against the messaging library
INCLUDEPATH += $$_PRO_FILE_PWD_/../../messaging
LIBS += -L$$OUT_PWD/../../messaging -lQtMessaging

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail

DEFINES += QMESSAGING_OPTIONAL QMESSAGING_OPTIONAL_FOLDER

DEFINES += SRCDIR=\\\"$$_PRO_FILE_PWD_\\\"
}

HEADERS += \
    addressfinder.h

SOURCES += \
    addressfinder.cpp\
    main.cpp

