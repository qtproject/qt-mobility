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
}
unix:!symbian {
    maemo5|maemo6 {
        SOURCES += tst_telephony_maemo.cpp
    } else {
        SOURCES += tst_telephony_linux.cpp
    }
}
symbian{
    SOURCES += tst_telephony_s60.cpp
}

addFiles.sources = testdata/*
addFiles.path = testdata
DEPLOYMENT += addFiles

CONFIG += mobility
MOBILITY = telephony
