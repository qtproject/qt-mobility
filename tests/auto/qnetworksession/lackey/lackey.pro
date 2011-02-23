SOURCES += main.cpp
TARGET = qnetworksessionlackey
INCLUDEPATH += ../../../../src/bearer
DEPENDPATH += ../../../../src/bearer

QT = core network

# Put the binary where tests can find it
CONFIG += testhelper

include(../../../../common.pri)

symbian {
     # Needed for interprocess communication and opening QNetworkSession
     TARGET.CAPABILITY = NetworkControl NetworkServices
}

CONFIG += mobility
MOBILITY = bearer
