REL_PATH_TO_SRC = ../../../src/telephony/

TARGET = tst_qtelephony
CONFIG+=testcase
QT = core

INCLUDEPATH += ../../../src/telephony
DEPENDPATH += ../../../src/telephony

include(../../../common.pri)

# Input
HEADERS += ../qsfwtestutil.h
SOURCES += ../qsfwtestutil.cpp


HEADERS += ../../../src/telephony/qtelephonycalllist.h \
           ../../../src/telephony/qtelephonycallinfo.h \
           ../../../src/telephony/qtelephonycallinfo_p.h

SOURCES += ../../../src/telephony/qtelephonycalllist.cpp \
           ../../../src/telephony/qtelephonycallinfo.cpp

# Private Headers and sources
win32 {
    SOURCES += tst_telephony_win.cpp
    HEADERS += ../../../src/telephony/qtelephonycalllist_win_p.h
    SOURCES += ../../../src/telephony/qtelephonycalllist_win.cpp
}

symbian {
    SOURCES += tst_telephony_s60.cpp
    HEADERS += ../../../src/telephony/qtelephonycalllist_symbian_p.h
    SOURCES += ../../../src/telephony/qtelephonycalllist_symbian.cpp
} else {
    unix: {
        !maemo* {
            SOURCES += tst_telephony_linux.cpp
            HEADERS += ../../../src/telephony/qtelephonycalllist_linux_p.h
            SOURCES += ../../../src/telephony/qtelephonycalllist_linux.cpp
        }
        maemo* {
            SOURCES += tst_telephony_maemo.cpp
            HEADERS += ../../../src/telephony/qtelephonycalllist_maemo_p.h
            SOURCES += ../../../src/telephony/qtelephonycalllist_maemo.cpp
        }
    }
}

addFiles.sources = testdata/*
addFiles.path = testdata
DEPLOYMENT += addFiles

CONFIG += mobility
