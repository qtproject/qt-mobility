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
            SOURCES += tst_telephony_linux.cpp
            HEADERS += ../../../src/telephony/qtelephonycalllist_linux_p.h
            SOURCES += ../../../src/telephony/qtelephonycalllist_linux.cpp
            HEADERS += ../../../src/telephony/qtelephonycalllist.h \
                       ../../../src/telephony/qtelephonycallinfo.h \
                       ../../../src/telephony/qtelephonycallinfo_p.h
            SOURCES += ../../../src/telephony/qtelephonycalllist.cpp \
                       ../../../src/telephony/qtelephonycallinfo.cpp
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
