TEMPLATE = app
include(../../examples.pri)

QT = core
CONFIG += no_icon
INCLUDEPATH += ../../../src/serviceframework

CONFIG += mobility
MOBILITY = serviceframework

SOURCES += exampleinstaller.cpp

symbian {
    TARGET.UID3 = 0xE00b7e42
    TARGET.CAPABILITY = ReadUserData WriteUserData
}
