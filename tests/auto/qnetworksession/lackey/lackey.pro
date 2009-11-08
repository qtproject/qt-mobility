SOURCES += main.cpp
TARGET = qnetworksessionlackey
INCLUDEPATH += ../../../../bearer
DEPENDPATH += ../../../../bearer

QT = core network

CONFIG+= testcase

include(../../../../common.pri)

qtAddLibrary(QtBearer)
