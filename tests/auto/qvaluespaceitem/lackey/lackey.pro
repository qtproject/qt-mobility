SOURCES += main.cpp
TARGET = vsiTestLackey
INCLUDEPATH += ../../../../context
QT = core

CONFIG+= testcase

include(../../../../common.pri)

qtAddLibrary(QtContextFramework)
