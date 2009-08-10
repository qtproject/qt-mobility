include(../../../common.pri)

TEMPLATE = app
TARGET = tst_qmessagestorekeys

CONFIG += testcase
QT += testlib

win32:debug{
    OUTPREFIX=Debug
} else:win32{
    OUTPREFIX=Release
}

# Build against the messaging library
INCLUDEPATH += $$_PRO_FILE_PWD_/../../../messaging
LIBS += -L$$OUT_PWD/../../../messaging/$$OUTPREFIX -lQtMessaging

# Link against the test support library
LIBS += -L$$OUT_PWD/../support/$$OUTPREFIX -lsupport

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail

DEFINES += QMESSAGING_OPTIONAL QMESSAGING_OPTIONAL_FOLDER
}

SOURCES += \
    tst_qmessagestorekeys.cpp

