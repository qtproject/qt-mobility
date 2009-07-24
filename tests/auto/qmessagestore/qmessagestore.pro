TEMPLATE = app
TARGET = tst_qmessagestore

CONFIG += testcase
QT += testlib

# Build against the messaging library
INCLUDEPATH += $$_PRO_FILE_PWD_/../../../messaging
LIBS += -L$$OUT_PWD/../../../messaging -lQtMessaging

# Link against the test support library
LIBS += -L$$OUT_PWD/../support -lsupport

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
}

SOURCES += \
    tst_qmessagestore.cpp

