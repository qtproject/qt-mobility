TARGET = tst_qtelephony
CONFIG+=testcase
QT = core

INCLUDEPATH += ../../../src/telephony
DEPENDPATH += ../../../src/telephony

include(../../../common.pri)

# Input
HEADERS += ../qsfwtestutil.h
SOURCES += ../qsfwtestutil.cpp

# Private Headers and sources
win32 {
    SOURCES += tst_telephony_win.cpp
    MOBILITY = telephony
}
symbian {
    SOURCES += tst_telephony_s60.cpp
    MOBILITY = telephony
} else {
    unix: {
        !maemo* {
            QT += dbus
            INCLUDEPATH += ../../../src/telephony/linux

            HEADERS += ../../../src/telephony/qtelephonycalllist.h \
                ../../../src/telephony/qtelephonycallinfo.h \
                ../../../src/telephony/qtelephonycallinfo_p.h \
                ../../../src/telephony/linux/qtelephonycalllist_linux_p.h \
                ../../../src/telephony/linux/telepathy.h \
                ../../../src/telephony/linux/telepathylistener.h \
                ../../../src/telephony/linux/message.h \
                ../../../src/telephony/linux/dbusadaptor.h \
                ../../../src/telephony/linux/dbusinterface.h

            SOURCES += tst_telephony_linux.cpp \
                ../../../src/telephony/qtelephonycalllist.cpp \
                ../../../src/telephony/qtelephonycallinfo.cpp \
                ../../../src/telephony/linux/telepathy.cpp \
                ../../../src/telephony/linux/telepathylistener.cpp \
                ../../../src/telephony/linux/message.cpp \
                ../../../src/telephony/linux/dbusadaptor.cpp \
                ../../../src/telephony/linux/dbusinterface.cpp \
                ../../../src/telephony/linux/qtelephonycalllist_linux.cpp
        }
        maemo* {
            SOURCES += tst_telephony_maemo.cpp
            MOBILITY = telephony
        }
    }
}

addFiles.sources = testdata/*
addFiles.path = testdata
DEPLOYMENT += addFiles

CONFIG += mobility
