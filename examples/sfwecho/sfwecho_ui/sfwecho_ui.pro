TARGET = sfwecho_ui
INCLUDEPATH += ../../../src/serviceframework
DEPENDPATH += ../../../src/serviceframework

QT = core gui
CONFIG +=
TEMPLATE = app

include(../../../common.pri)

include(../../examples.pri)

CONFIG += mobility
MOBILITY = serviceframework

SOURCES += main.cpp
HEADERS += 

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
