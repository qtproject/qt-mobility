include(declarative-sfw-notes.pri)
include(deployment.pri)

INCLUDEPATH += ../../src/serviceframework
DEPENDPATH += ../../src/serviceframework

QT += declarative network script
SOURCES += qmlnotes.cpp
TARGET = qmlnotes
RESOURCES += declarative-sfw-notes.qrc

CONFIG += mobility
MOBILITY += serviceframework

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
