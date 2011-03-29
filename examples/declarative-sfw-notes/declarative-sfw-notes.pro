include(../mobility_examples.pri)

INCLUDEPATH += ../../src/serviceframework
DEPENDPATH += ../../src/serviceframework

QT += declarative network script
SOURCES += qmlnotes.cpp
TARGET = qmlnotes
TEMPLATE = app
RESOURCES += declarative-sfw-notes.qrc

CONFIG += mobility
MOBILITY += serviceframework

symbian {
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
