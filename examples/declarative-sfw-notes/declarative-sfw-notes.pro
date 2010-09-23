include(declarative-sfw-notes.pri)
include(deployment.pri)

QT += declarative network script
SOURCES += qmlnotes.cpp
TARGET = qmlnotes

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
