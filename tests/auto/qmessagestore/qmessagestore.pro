TEMPLATE = app
TARGET = tst_qmessagestore

CONFIG += testcase
QT += testlib

include(../../../common.pri)

# Build against the messaging library
INCLUDEPATH += $$SOURCE_DIR/messaging
LIBS += -lQtMessaging

# Link against the test support library
LIBS += -lsupport

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
}

DEFINES += SRCDIR=\\\"$$_PRO_FILE_PWD_\\\"

SOURCES += \
    tst_qmessagestore.cpp

