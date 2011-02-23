SOURCES += main.cpp
TARGET = vsiTestLackey
INCLUDEPATH += ../../../../src/publishsubscribe
DEPENDPATH += ../../../../src/publishsubscribe

QT = core

# Put the binary where tests can find it
CONFIG += testhelper

include(../../../../common.pri)

CONFIG += mobility
MOBILITY = publishsubscribe
