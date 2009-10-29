SOURCES += main.cpp
TARGET = qnetworksessionlackey
INCLUDEPATH += ../../../../bearer
DEPENDPATH += ../../../../bearer

QT = core

CONFIG+= testcase

include(../../../../common.pri)

qtAddLibrary(QtBearer)
