QT = core

include(../../common.pri)

TARGET = nfctestserver
CONFIG += console
CONFIG -= app_bundle
CONFIG += mobility strict_flags

INCLUDEPATH += $$PWD/../../src/connectivity/nfc
DEPENDPATH += $$PWD/../../src/connectivity/nfc
MOBILITY += connectivity

TEMPLATE = app

SOURCES += main.cpp
