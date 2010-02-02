SOURCES += main.cpp
TARGET = qnetworksessionlackey
INCLUDEPATH += ../../../../src/bearer
DEPENDPATH += ../../../../src/bearer

QT = core network

CONFIG+= testcase

include(../../../../common.pri)

symbian {
    TARGET.CAPABILITY = All -TCB
}

CONFIG += mobility
MOBILITY = bearer
