include(../../../common.pri)

TEMPLATE = app
TARGET = tst_qmessage
target.path += $$TESTS_INSTALL_ROOT
INSTALLS += target


CONFIG += testcase
QT += testlib

# Build against the messaging library
INCLUDEPATH += $$_PRO_FILE_PWD_/../../../messaging
LIBS += -L$$OUT_PWD/../../../messaging/$$OUTPREFIX -lQtMessaging

# Link against the test support library
LIBS += -L$$OUT_PWD/../support/$$OUTPREFIX -lsupport

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
DEFINES += SRCDIR=\\\"$$_PRO_FILE_PWD_\\\"
}

SOURCES += \
    tst_qmessage.cpp

