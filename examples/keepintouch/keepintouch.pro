include(../../common.pri)

TEMPLATE = app
TARGET = keepintouch
target.path += $$EXAMPLES_INSTALL_ROOT
INSTALLS += target

QT += gui

win32:debug {
    OUTPREFIX=Debug
} else:win32 {
    OUTPREFIX=Release
}

# Build against the messaging library
INCLUDEPATH += $$_PRO_FILE_PWD_/../../messaging
LIBS += -L$$OUT_PWD/../../messaging/$$OUTPREFIX -lQtMessaging

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
DEFINES += SRCDIR=\\\"$$_PRO_FILE_PWD_\\\"
}

HEADERS += \
    addressfinder.h

SOURCES += \
    addressfinder.cpp\
    main.cpp

