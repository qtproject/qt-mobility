TEMPLATE = app
TARGET = querymessages

QT += gui

include(../../common.pri)

qtAddLibrary(QtMessaging)
INCLUDEPATH += ../../messaging

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
}

SOURCES += \
    main.cpp

