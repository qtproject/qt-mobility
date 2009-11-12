SOURCES += main.cpp
TARGET = vsiTestLackey
INCLUDEPATH += ../../../../context
DEPENDPATH += ../../../../context

QT = core

CONFIG+= testcase

include(../../../../common.pri)

qtAddLibrary(QtPublishSubscribe)
