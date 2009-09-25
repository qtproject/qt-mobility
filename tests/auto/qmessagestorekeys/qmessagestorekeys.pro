TEMPLATE = app
TARGET = tst_qmessagestorekeys

CONFIG += testcase
QT += testlib

include(../../../common.pri)

qtAddLibrary(QtMessaging)
INCLUDEPATH += ../../../messaging

# Link against the test support library
LIBS += -lsupport

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
}

SOURCES += \
    tst_qmessagestorekeys.cpp

