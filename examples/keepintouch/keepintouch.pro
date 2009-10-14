TEMPLATE = app
TARGET = keepintouch

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
    addressfinder.h


SOURCES += \
    addressfinder.cpp\
    main.cpp

