TEMPLATE = app
include(../../examples.pri)

QT = core
CONFIG += no_icon
INCLUDEPATH += ../../../src/serviceframework

qtAddLibrary(QtServiceFramework)

SOURCES += exampleinstaller.cpp

symbian {
    TARGET.UID3 = 0xE00b7e42
    TARGET.CAPABILITY = ALL -TCB
}
