TEMPLATE = app
TARGET = tst_qmessagestorekeys

CONFIG += testcase
QT += testlib

include(../../../common.pri)
include(../support/support.pri)

qtAddLibrary(QtMessaging)
INCLUDEPATH += ../../../messaging

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
}

SOURCES += \
    tst_qmessagestorekeys.cpp

