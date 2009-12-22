SOURCES += main.cpp
TARGET = vsiTestLackey
INCLUDEPATH += ../../../../src/publishsubscribe
DEPENDPATH += ../../../../src/publishsubscribe

QT = core

CONFIG+= testcase

include(../../../../common.pri)

CONFIG += mobility
MOBILITY = publishsubscribe
